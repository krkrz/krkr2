//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "mycrypt.h"
#include "KrkrExecutableUnit.h"
#include "KrkrSignatureUnit.h"


//---------------------------------------------------------------------------
#define HASH_INIT    md5_init
#define HASH_PROCESS md5_process
#define HASH_DONE    md5_done
#define HASH_METHOD_STRING "MD5"
#define HASH_SIZE     16
//---------------------------------------------------------------------------
static bool MakeFileHash(AnsiString fn, unsigned char *hash, int ignorestart,
	int ignoreend, TSigProgressNotifyEvent notify)
{
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
static bool CheckSignatureOfFile(AnsiString pubkey,
	AnsiString signfn, int ignorestart, int ignoreend, int ofs,
		TSigProgressNotifyEvent notify)
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
		bool interrupted = MakeFileHash(signfn, hash, ignorestart, ignoreend,
			notify);
		if(!interrupted)
		{
			// check signature
			buf_len = sizeof(buf);
			errnum = ecc_verify_hash(buf, buf_len, hash, HASH_SIZE, &stat, &key);
			if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
		}
		else
		{
			stat = 0;
		}
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

