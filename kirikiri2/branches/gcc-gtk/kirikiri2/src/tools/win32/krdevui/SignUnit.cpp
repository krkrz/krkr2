//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SignUnit.h"
#include "RandomizeFormUnit.h"
#include "mycrypt.h"

#include <string.h>

#define HASH_INIT    md5_init
#define HASH_PROCESS md5_process
#define HASH_DONE    md5_done
#define HASH_METHOD_STRING "MD5"
#define HASH_SIZE     16

//---------------------------------------------------------------------------
static void MakeFileHash(AnsiString fn, unsigned char *hash, int ignorestart,
	int ignoreend)
{
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
bool SignFile(AnsiString privkey, AnsiString signfn, int ignorestart, int ignoreend, int ofs)
{
	// ofs is -1  : for separeted sinature file (.sig) .
	// otherwise the sign is embedded to the target file.

	// sign file
	if(privkey == "") throw Exception("Specify private key");
	if(signfn == "") throw Exception("Specify target file");

	// read privkey
	AnsiString privkeymark("-- PRIVATE KEY - ECC(160) --");
	if(privkey.Length() <  privkeymark.Length())
		throw Exception("Invalid private key");
	if(strncmp(privkey.c_str(), privkeymark.c_str(), privkeymark.Length()))
		throw Exception("Invalid private key");

	ecc_key key;
	int errnum;
	char buf[1024];
	unsigned long buf_len;

	buf_len = sizeof(buf) - 1;
	errnum = base64_decode((const unsigned char*)(privkey.c_str() + privkeymark.Length()),
		privkey.Length() - privkeymark.Length(), buf, &buf_len);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

	errnum = ecc_import(buf, buf_len, &key);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

	try
	{
		// initialize random generator
		prng_state prng;
		if(!RandomizePRNGSimple(&prng)) return false;

		// make target hash
		unsigned char hash[HASH_SIZE];
		MakeFileHash(signfn, hash, ignorestart, ignoreend);

		// sign
		buf_len = sizeof(buf) - 1;
		errnum = ecc_sign_hash(hash, HASH_SIZE, buf, &buf_len, &prng, find_prng("yarrow"), &key);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

		// convert to readable text
		char buf_asc[1024*3/2+2];
		unsigned long buf_asc_len;
		buf_asc_len = sizeof(buf_asc) - 1;
		errnum = base64_encode(buf, buf_len, buf_asc, &buf_asc_len);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
		buf_asc[buf_asc_len] = 0;

		AnsiString sign = AnsiString("-- SIGNATURE - ECC(160)/" HASH_METHOD_STRING " --") + buf_asc;

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
		ecc_free(&key);
		throw;
	}
	ecc_free(&key);

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
	char buf[1024];
	unsigned long buf_len;
	char buf_asc[1024*3/2+2];
	unsigned long buf_asc_len;

	AnsiString pubkeymark("-- PUBLIC KEY - ECC(160) --");
	if(pubkey.Length() <  pubkeymark.Length())
		throw Exception("Invalid public key");
	if(strncmp(pubkey.c_str(), pubkeymark.c_str(), pubkeymark.Length()))
		throw Exception("Invalid public key");

	ecc_key key;

	buf_len = sizeof(buf) - 1;
	int errnum;
	errnum = base64_decode((const unsigned char*)(pubkey.c_str() + pubkeymark.Length()),
		pubkey.Length() - pubkeymark.Length(), buf, &buf_len);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

	errnum = ecc_import(buf, buf_len, &key);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

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

	AnsiString signmark("-- SIGNATURE - ECC(160)/" HASH_METHOD_STRING " --");
	if(strncmp(buf_asc, signmark.c_str(), signmark.Length()))
		throw Exception("Invalid signature file format");

	buf_len = sizeof(buf) - 1;
	errnum = base64_decode((const unsigned char*)(buf_asc + signmark.Length()),
		buf_asc_len - signmark.Length(), buf, &buf_len);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));

	int stat = 0;
	try
	{
		// make target hash
		unsigned char hash[HASH_SIZE];
		MakeFileHash(signfn, hash, ignorestart, ignoreend);

		// check signature
		buf_len = sizeof(buf);
		errnum = ecc_verify_hash(buf, buf_len, hash, HASH_SIZE, &stat, &key);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
	}
	catch(...)
	{
		ecc_free(&key);
		throw;
	}
	ecc_free(&key);

	return stat;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
