//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#define _MSC_VER
#include <tomcrypt.h>
#undef _MSC_VER

#include "KrkrExecutableUnit.h"
#include "KrkrSignatureUnit.h"


//---------------------------------------------------------------------------
#define HASH_INIT    sha256_init
#define HASH_PROCESS sha256_process
#define HASH_DONE    sha256_done
#define HASH_DESC    sha256_desc
#define HASH_METHOD_STRING "SHA256"
#define HASH_METHOD_INTERNAL_STRING "sha256"
#define HASH_SIZE     32
//---------------------------------------------------------------------------
static bool MakeFileHash(AnsiString fn, unsigned char *hash, int ignorestart,
	int ignoreend, TSigProgressNotifyEvent notify)
{
	if(find_hash(HASH_METHOD_INTERNAL_STRING) == -1)
	{
		int errnum = register_hash(&HASH_DESC);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
	}


	if(notify)
		if(!notify(0)) return true;

	int pts = 0;

	TFileStream *fs;
	fs = new TFileStream(fn, fmOpenRead | fmShareDenyWrite);
	if(ignorestart != -1 && ignoreend == -1) ignoreend = fs->Size;
	int signfnsize = fs->Size;
	bool interrupted = false;
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

			if(notify)
			{
				// callback notify
				int npts = (int)( (__int64)ofs * (__int64)100 / (__int64)signfnsize );
				if(pts < npts)
				{
					pts = npts;
					if(!notify(pts))
					{
						interrupted = true;
						break;
					}
				}
			}
		}

		if(!interrupted && ignorestart != -1 && fs->Position != ignoreend)
		{
			fs->Position = ofs = ignoreend;

			while((read = fs->Read(buf, sizeof(buf))) != 0)
			{
				HASH_PROCESS(&st, buf, read);
				ofs += read;

				if(notify)
				{
					// callback notify
					int npts = (int)( (__int64)ofs * (__int64)100 / (__int64)signfnsize );
					if(pts < npts)
					{
						pts = npts;
						if(!notify(pts))
						{
							interrupted = true;
							break;
						}
					}
				}
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


	if(!interrupted && notify)
		if(!notify(100)) return true;

	return interrupted;
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
static bool CheckSignatureOfFile(AnsiString pubkey,
	AnsiString signfn, int ignorestart, int ignoreend, int ofs,
		TSigProgressNotifyEvent notify)
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
		st = new TFileStream(signfn  + ".sig", fmOpenRead);
	}
	else
	{
		// embedded
		st = new TFileStream(signfn, fmOpenRead);
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
		bool interrupted = MakeFileHash(signfn, hash, ignorestart, ignoreend,
			notify);
		if(!interrupted)
		{
			// check signature
			errnum = rsa_verify_hash(buf, buf_len, hash, HASH_SIZE,
				find_hash(HASH_METHOD_INTERNAL_STRING), HASH_SIZE,
				&stat, &key);
			if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
		}
		else
		{
			stat = 0;
		}
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


//---------------------------------------------------------------------------
extern bool CheckExeHasSignature(AnsiString fn)
{
	// check whether "fn" has executable signature of KIRIKIRI.
	return CheckKrkrExecutable(fn, XRELEASE_SIG____) != 0;
}
//---------------------------------------------------------------------------
extern bool CheckSignature(AnsiString fn, AnsiString publickey,
	TSigProgressNotifyEvent notify)
{
	bool result;

	int ignorestart = CheckKrkrExecutable(fn, XOPT_EMBED_AREA_);
	int signofs = CheckKrkrExecutable(fn, XRELEASE_SIG____);
	int xp3ofs = CheckKrkrExecutable(fn, XP3_SIG);

	if(ignorestart != 0 && signofs != 0)
	{
		// krkr executable
		result = CheckSignatureOfFile(publickey, fn, ignorestart,
			xp3ofs?xp3ofs:-1, signofs+ 16+4, notify);
	}
	else
	{
		// normal file
		result = CheckSignatureOfFile(publickey, fn, -1, -1, -1, notify);
	}

	return result;
}
//---------------------------------------------------------------------------

