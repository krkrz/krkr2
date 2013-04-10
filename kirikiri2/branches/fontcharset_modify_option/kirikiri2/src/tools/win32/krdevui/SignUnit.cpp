//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SignUnit.h"
#include "RandomizeFormUnit.h"

#define _MSC_VER
#include <tomcrypt.h>
#undef _MSC_VER


#include <string.h>

#define HASH_INIT    sha256_init
#define HASH_PROCESS sha256_process
#define HASH_DONE    sha256_done
#define HASH_DESC    sha256_desc
#define HASH_METHOD_STRING "SHA256"
#define HASH_METHOD_INTERNAL_STRING "sha256"
#define HASH_SIZE     32

//---------------------------------------------------------------------------
AnsiString Split64(AnsiString s)
{
	// split s in 64characters intervally
	int len = s.Length();
	const char * p = s.c_str();
	AnsiString ret;
	while(len > 0)
	{
		int one_size = len > 64 ? 64  : len;
		ret += AnsiString(p, one_size) + "\r\n";
		p += one_size;
		len -= one_size;
	}
	return ret;
}
//---------------------------------------------------------------------------
static void MakeFileHash(AnsiString fn, unsigned char *hash, int ignorestart,
	int ignoreend)
{
	if(find_hash(HASH_METHOD_INTERNAL_STRING) == -1)
	{
		int errnum = register_hash(&HASH_DESC);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
	}


	TFileStream *fs;
	fs = new TFileStream(fn, fmOpenRead | fmShareDenyWrite);
	if(ignorestart != -1 && ignoreend == -1) ignoreend = fs->Size;
	try
	{
		hash_state st;
		HASH_INIT(&st);

		int read;
		unsigned char buf[4096];
		int ofs = 0;
		while((read = fs->Read(buf, sizeof(buf))) != 0)
		{
			if(ignorestart != -1 && read + ofs > ignorestart)
			{
				read = ignorestart - ofs;
				if(read) HASH_PROCESS(&st, buf, read);
				break;
			}
			else
			{
				HASH_PROCESS(&st, buf, read);
			}
			ofs += read;
		}

		if(ignorestart != -1 && fs->Position != ignoreend)
		{
			fs->Position = ofs = ignoreend;

			while((read = fs->Read(buf, sizeof(buf))) != 0)
			{
				HASH_PROCESS(&st, buf, read);
				ofs += read;
			}
		}

		HASH_DONE(&st, hash);
	}
	catch(...)
	{
		delete fs;
		throw;
	}
	delete fs;
}
//---------------------------------------------------------------------------
static void ImportKey(AnsiString inkey, AnsiString startline, AnsiString endline,
	rsa_key * key)
{
	const char *pkey = inkey.c_str();
	const char *start = strstr(pkey, startline.c_str());
	if(!start) throw Exception("Cannot find \"" + startline + "\" in the key string");
	const char *end = strstr(pkey, endline.c_str());
	if(!end) throw Exception("Cannot find \"" + endline + "\" in the key string");

	start += startline.Length();
	char buf[10240];
	unsigned long buf_len;
	int errnum;

	buf_len = sizeof(buf) - 1;
	errnum = base64_decode((const unsigned char*)(start),
		end - start, buf, &buf_len);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

	errnum = rsa_import(buf, buf_len, key);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
}
//---------------------------------------------------------------------------
bool SignFile(AnsiString privkey, AnsiString signfn, int ignorestart, int ignoreend, int ofs)
{
	// ofs is -1  : for separeted sinature file (.sig) .
	// otherwise the sign is embedded to the target file.

	// sign file
	if(privkey == "") throw Exception("Specify private key");
	if(signfn == "") throw Exception("Specify target file");

	// read privkey
	rsa_key key;
	int errnum;

	ImportKey(privkey, "-----BEGIN RSA PRIVATE KEY-----", "-----END RSA PRIVATE KEY-----",
		&key);

	try
	{
		char buf[10240];
		unsigned long buf_len;

		// initialize random number generator
		prng_state prng;
		if(!RandomizePRNGSimple(&prng)) return false;

		// make target hash
		unsigned char hash[HASH_SIZE];
		MakeFileHash(signfn, hash, ignorestart, ignoreend);

		// sign
		buf_len = sizeof(buf) - 1;
		errnum = rsa_sign_hash(hash, HASH_SIZE, buf, &buf_len,
			&prng, find_prng("fortuna"),
			find_hash(HASH_METHOD_INTERNAL_STRING), HASH_SIZE,
			&key);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

		// convert to readable text
		char buf_asc[10240*3/2+2];
		unsigned long buf_asc_len;
		buf_asc_len = sizeof(buf_asc) - 1;
		errnum = base64_encode(buf, buf_len, buf_asc, &buf_asc_len);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
		buf_asc[buf_asc_len] = 0;

		AnsiString sign = AnsiString("-- SIGNATURE - " HASH_METHOD_STRING "/PSS/RSA --\r\n") +
			Split64(buf_asc);

		// write it to the file
		TFileStream * st;

		if(ofs == -1)
		{
			// separated
			st = new TFileStream(signfn  + ".sig", fmCreate|fmShareDenyWrite);
		}
		else
		{
			// embedded
			st = new TFileStream(signfn, fmOpenReadWrite|fmShareDenyWrite);
			st->Position = ofs;
		}

		try
		{
			st->Write(sign.c_str(), sign.Length());
			if(ofs != -1)
			{
				// write a null terminater
				st->Write("\0", 1);
			}
		}
		catch(...)
		{
			delete st;
			throw;
		}
		delete st;
	}
	catch(...)
	{
		rsa_free(&key);
		throw;
	}
	rsa_free(&key);

	return true;
}
//---------------------------------------------------------------------------
bool CheckSignatureOfFile(AnsiString pubkey, AnsiString signfn,
	int ignorestart, int ignoreend, int ofs)
{
	// check signature of the file
	if(pubkey == "") throw Exception("Specify public key");
	if(signfn == "") throw Exception("Specify target file");

	// read pubkey
	char buf[10240];
	unsigned long buf_len;
	char buf_asc[sizeof(buf)*3/2+2];
	unsigned long buf_asc_len;

	rsa_key key;

	ImportKey(pubkey, "-----BEGIN PUBLIC KEY-----", "-----END PUBLIC KEY-----",
		&key);

	// read signature file

	TFileStream *st;
	if(ofs == -1)
	{
		// separated
		st = new TFileStream(signfn  + ".sig", fmOpenRead|fmShareDenyWrite);
	}
	else
	{
		// embedded
		st = new TFileStream(signfn, fmOpenReadWrite|fmShareDenyWrite);
		st->Position = ofs;
	}
	try
	{
		buf_asc_len = st->Read(buf_asc, sizeof(buf_asc) - 1);
	}
	catch(...)
	{
		delete st;
		throw;
	}
	delete st;

	buf_asc[buf_asc_len] = 0;
	buf_asc_len = strlen(buf_asc);

	AnsiString signmark("-- SIGNATURE - " HASH_METHOD_STRING "/PSS/RSA --");
	if(strncmp(buf_asc, signmark.c_str(), signmark.Length()))
		throw Exception("Invalid signature file format");

	buf_len = sizeof(buf) - 1;
	int errnum = base64_decode((const unsigned char*)(buf_asc + signmark.Length()),
		buf_asc_len - signmark.Length(), buf, &buf_len);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

	int stat = 0;
	try
	{
		// make target hash
		unsigned char hash[HASH_SIZE];
		MakeFileHash(signfn, hash, ignorestart, ignoreend);

		// check signature
		errnum = rsa_verify_hash(buf, buf_len, hash, HASH_SIZE,
			find_hash(HASH_METHOD_INTERNAL_STRING), HASH_SIZE,
			&stat, &key);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
	}
	catch(...)
	{
		rsa_free(&key);
		throw;
	}
	rsa_free(&key);

	return stat;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
