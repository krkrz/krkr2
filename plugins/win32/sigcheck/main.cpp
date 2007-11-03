#include "ncbind/ncbind.hpp"

#include <stdio.h>
#include <string>
using namespace std;

// copy from 
// kirikiri\kirikiri2\src\tools\win32\sigchecker\krkrsiglib

//---------------------------------------------------------------------------
#define MIN_KRKR_MARK_SEARCH 512*1024
#define MAX_KRKR_MARK_SEARCH 4*1024*1024
//---------------------------------------------------------------------------
char XOPT_EMBED_AREA_[] = " OPT_EMBED_AREA_";
char XCORE_SIG_______[] = " CORE_SIG_______";
char XRELEASE_SIG____[] = " RELEASE_SIG____";
char XP3_SIG[] = " P3\x0d\x0a\x20\x0a\x1a\x8b\x67\x01";   // mark_size = 11
//---------------------------------------------------------------------------

// IStream の読み込み位置取得
static DWORD getPosition(IStream *is)
{
	if (is) {
		LARGE_INTEGER move = {0};
		ULARGE_INTEGER newposition;
		if (is->Seek(move, STREAM_SEEK_CUR, &newposition) == S_OK) {
			return (DWORD)newposition.QuadPart;
		}
	}
	return -1;

}

// IStream の読み込み位置指定
static void setPosition(IStream *is, DWORD offset)
{
	if (is) {
		LARGE_INTEGER move;
		move.QuadPart = offset;
		ULARGE_INTEGER newposition;
		is->Seek(move, STREAM_SEEK_SET, &newposition);
	}
}

// IStream のサイズ取得
static DWORD getSize(IStream *is)
{
	if (is) {
		STATSTG stat;
		is->Stat(&stat, STATFLAG_NONAME);
		return (DWORD)stat.cbSize.QuadPart;
	}
	return 0;
}

static int CheckKrkrExecutable(ttstr &fn, const char *mark)
{
	// Find the mark in the krkr executable.
	// All krkr executable have
	// "XOPT_EMBED_AREA_" mark at the end of the executable image.
	// Additionally
	// "XCORE_SIG_______" and "XRELEASE_SIG____" are reserved area for
	// signatures.
	// "XP3\x0d\x0a\x20\x0a\x1a\x8b\x67\x01" are optional XP3 archive attached
	// to the executable.

	// This function returns the mark offset
	// (area size bofore specified mark)
	int mark_size = strlen(mark);

	IStream *st = TVPCreateIStream(fn, TJS_BS_READ);
	if (st == NULL) {
		TVPThrowExceptionMessage((ttstr(L"can't open file:") + fn).c_str());
	}
	
	int imagesize = 0;
	try
	{
		int ofs = MIN_KRKR_MARK_SEARCH;
		char buf[4096];
		DWORD read;
		bool found = false;
		setPosition(st, ofs);
		while( st->Read(buf, sizeof(buf), &read) == S_OK && read != 0)
		{
			for(int i = 0; i < read; i += 16) // the mark is aligned to paragraph (16bytes)
			{
				if(buf[i] == 'X')
				{
					if(!memcmp(buf + i + 1, mark + 1, mark_size - 1))
					{
						// mark found
						imagesize = i + ofs;
						found = true;
						break;
					}
				}
			}
			if(found) break;
			ofs += read;
			if(ofs >= MAX_KRKR_MARK_SEARCH) break;
		}
	}
	catch(...)
	{
		st->Release();
		throw;
	}
	st->Release();

	return imagesize;
}

//---------------------------------------------------------------------------

#include <tomcrypt.h>

//---------------------------------------------------------------------------

#undef HASH_PROCESS

#define HASH_INIT    sha256_init
#define HASH_PROCESS sha256_process
#define HASH_DONE    sha256_done
#define HASH_DESC    sha256_desc
#define HASH_METHOD_STRING "SHA256"
#define HASH_METHOD_INTERNAL_STRING "sha256"
#define HASH_SIZE     32


bool callNotify(iTJSDispatch2 *notify, int param)
{
	tTJSVariant result;
	tTJSVariant var = tTJSVariant(param);
	tTJSVariant *vars[] = {&var};
	notify->FuncCall(0, L"onCheckSignatureProgress", NULL, &result, 1, vars, notify);
	return (int)result != 0;
}

//---------------------------------------------------------------------------
static bool MakeFileHash(ttstr &fn, unsigned char *hash,
						 int ignorestart, int ignoreend, iTJSDispatch2 *notify)
{
	if(find_hash(HASH_METHOD_INTERNAL_STRING) == -1)
	{
		int errnum = register_hash(&HASH_DESC);
		if(errnum != CRYPT_OK) {
			TVPThrowExceptionMessage(ttstr(error_to_string(errnum)).c_str());
		}
	}

	if (notify) {
		if (!callNotify(notify, 0)) {
			return true;
		}
	}

	int pts = 0;

	IStream *fs = TVPCreateIStream(fn, TJS_BS_READ);
	if (fs == NULL) {
		TVPThrowExceptionMessage((ttstr(L"can't open file:") + fn).c_str());
	}

	int size = getSize(fs);
	if(ignorestart != -1 && ignoreend == -1) ignoreend = size;
	int signfnsize = size;
	bool interrupted = false;
	try
	{
		hash_state st;
		HASH_INIT(&st);

		DWORD read;
		unsigned char buf[4096];
		int ofs = 0;
		while(fs->Read(buf, sizeof(buf), &read) == S_OK && read != 0)
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
					if (!callNotify(notify, 0)) {
						interrupted = true;
						break;
					}
				}
			}
		}

		if(!interrupted && ignorestart != -1 && getPosition(fs) != ignoreend)
		{
			setPosition(fs, (ofs = ignoreend));
			
			while(fs->Read(buf, sizeof(buf), &read) == S_OK && read != 0)
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
						if (!callNotify(notify, 0)) {
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
		fs->Release();
		throw;
	}
	fs->Release();

	if(!interrupted && notify)
		if(!callNotify(notify,100)) return true;

	return interrupted;
}
//---------------------------------------------------------------------------
static void ImportKey(const char *inkey, const char *startline, const char *endline, rsa_key * key)
{
	const char *start = strstr(inkey, startline);
	if (!start) {
		TVPThrowExceptionMessage((ttstr(L"Cannot find \"") + startline + "\" in the key string").c_str());
	}
	const char *end = strstr(inkey, endline);
	if (!end) {
		TVPThrowExceptionMessage((ttstr(L"Cannot find \"") + endline + "\" in the key string").c_str());
	}
	start += strlen(startline);
	unsigned char buf[10240];
	unsigned long buf_len;
	int errnum;

	buf_len = sizeof(buf) - 1;
	errnum = base64_decode((const unsigned char*)(start), end - start, buf, &buf_len);
	if(errnum != CRYPT_OK) {
		TVPThrowExceptionMessage(ttstr(error_to_string(errnum)).c_str());
	}
	errnum = rsa_import(buf, buf_len, key);
	if(errnum != CRYPT_OK) {
		TVPThrowExceptionMessage(ttstr(error_to_string(errnum)).c_str());
	}
}
//---------------------------------------------------------------------------
static int CheckSignatureOfFile(ttstr &signfn,
								const char *pubkey,
								int ignorestart, int ignoreend, int ofs,
								iTJSDispatch2 *notify)
{
	// check signature of the file
	if(pubkey == NULL || *pubkey == '\0') {
		TVPThrowExceptionMessage(L"Specify public key");
	}
	if(signfn == "") {
		TVPThrowExceptionMessage(L"Specify target file");
	}

	// read pubkey
	unsigned char buf[10240];
	unsigned long buf_len;
	char buf_asc[sizeof(buf)*3/2+2];
	unsigned long buf_asc_len;

	rsa_key key;

	ImportKey(pubkey, "-----BEGIN PUBLIC KEY-----", "-----END PUBLIC KEY-----",	&key);

	// read signature file

	IStream *st = NULL;
	if(ofs == -1)
	{
		// separated
		st = TVPCreateIStream(signfn + L".sig", TJS_BS_READ);
	}
	else
	{
		// embedded
		st = TVPCreateIStream(signfn, TJS_BS_READ);
		setPosition(st, ofs);
	}
	if (st == NULL) {
		// 署名ファイルが開けないので失敗
		return false;
	}

	try
	{
		st->Read(buf_asc, sizeof(buf_asc) - 1, &buf_asc_len);
	}
	catch(...)
	{
		st->Release();
		throw;
	}
	st->Release();

	buf_asc[buf_asc_len] = 0;
	buf_asc_len = strlen(buf_asc);

	string signmark("-- SIGNATURE - " HASH_METHOD_STRING "/PSS/RSA --");
	if(strncmp(buf_asc, signmark.data(), signmark.size())) {
		TVPThrowExceptionMessage(L"Invalid signature file format");
	}

	buf_len = sizeof(buf) - 1;
	int errnum = base64_decode((const unsigned char*)(buf_asc + signmark.size()),
		buf_asc_len - signmark.size(), buf, &buf_len);
	if(errnum != CRYPT_OK) {
		TVPThrowExceptionMessage(ttstr(error_to_string(errnum)).c_str());
	}

	int stat = 0;
	try
	{
		// make target hash
		unsigned char hash[HASH_SIZE];
		bool interrupted = MakeFileHash(signfn, hash, ignorestart, ignoreend, notify);
		if(!interrupted)
		{
			// check signature
			errnum = rsa_verify_hash(buf, buf_len, hash, HASH_SIZE,
				find_hash(HASH_METHOD_INTERNAL_STRING), HASH_SIZE,
				&stat, &key);
			if(errnum != CRYPT_OK) {
				TVPThrowExceptionMessage(ttstr(error_to_string(errnum)).c_str());
			}
		}
		else
		{
			stat = -1;
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
static bool CheckExeHasSignature(ttstr &fn)
{
	// check whether "fn" has executable signature of KIRIKIRI.
	return CheckKrkrExecutable(fn, XRELEASE_SIG____) != 0;
}

//---------------------------------------------------------------------------

/**
 *
 * @return 結果 -1:中断 0:失敗 1:成功
 */
static int CheckSignature(ttstr &fn,
						  const char *publickey,
						  iTJSDispatch2 *notify)
{
	int ignorestart = CheckKrkrExecutable(fn, XOPT_EMBED_AREA_);
	int signofs     = CheckKrkrExecutable(fn, XRELEASE_SIG____);
	int xp3ofs      = CheckKrkrExecutable(fn, XP3_SIG);
	
	if(ignorestart != 0 && signofs != 0)
	{
		// krkr executable
		return CheckSignatureOfFile(fn, publickey, ignorestart,
									xp3ofs?xp3ofs:-1, signofs+ 16+4, notify);
	}
	else
	{
		// normal file
		return CheckSignatureOfFile(fn, publickey, -1, -1, -1, notify);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
 * C文字列処理用
 */
class NarrowString {
private:
	tjs_nchar *_data;
public:
	NarrowString(ttstr &str) : _data(NULL) {
		tjs_int len = str.GetNarrowStrLen();
		_data = new tjs_nchar[len+1];
		str.ToNarrowStr(_data, len+1);
	}
	~NarrowString() {
		delete[] _data;
	}

	const tjs_nchar *data() {
		return _data;
	}

	operator const char *() const
	{
		return (const char *)_data;
	}
};

struct StoragesSigCheck {
	/**
	 * 署名チェックを行う
	 * @param filename 対象ファイル
	 * @param publickey 公開鍵
	 */
	static tjs_error TJS_INTF_METHOD checkSignature(tTJSVariant *result,
													tjs_int numparams,
													tTJSVariant **param,
													iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		ttstr filename  = *param[0];
		ttstr publickey = *param[1];
		iTJSDispatch2 *notify = (numparams > 2 && param[2]->Type() == tvtObject) ?
			param[2]->AsObjectNoAddRef() : NULL;
		int ret = CheckSignature(filename, NarrowString(publickey), notify);
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(StoragesSigCheck, Storages) {
	RawCallback("checkSignature", &ClassT::checkSignature, TJS_STATICMEMBER);
};

/**
 * 登録処理後
 */
static void PostRegistCallback()
{
	ltc_mp = ltm_desc;
}

NCB_POST_REGIST_CALLBACK(PostRegistCallback);
