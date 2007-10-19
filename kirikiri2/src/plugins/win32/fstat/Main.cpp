#include "ncbind/ncbind.hpp"

// Date クラスメンバ
static iTJSDispatch2 *dateClass = NULL;    // Date のクラスオブジェクト
static iTJSDispatch2 *dateSetTime = NULL;  // Date.setTime メソッド

/**
 * ファイル時刻を Date クラスにして保存
 * @param store 格納先
 * @param filetime ファイル時刻
 */
static void storeDate(tTJSVariant &store, FILETIME &filetime, iTJSDispatch2 *objthis)
{
	// ファイル生成時
	tjs_uint64 ft = filetime.dwHighDateTime * 0x100000000 | filetime.dwLowDateTime;
	if (ft > 0) {
		iTJSDispatch2 *obj;
		if (TJS_SUCCEEDED(dateClass->CreateNew(0, NULL, NULL, &obj, 0, NULL, objthis))) {
			// UNIX TIME に変換
			tjs_int64 unixtime = (ft - 0x19DB1DED53E8000 ) / 10000;
			tTJSVariant time(unixtime);
			tTJSVariant *param[] = { &time };
			dateSetTime->FuncCall(0, NULL, NULL, NULL, 1, param, obj);
			store = tTJSVariant(obj, obj);
			obj->Release();
		}
	}
}

/**
 * メソッド追加用
 */
class StoragesFstat {

public:
	StoragesFstat(){};

	/**
	 * 指定されたファイルの情報を取得する
	 * @param filename ファイル名
	 * @return 
	 */
	static tjs_error TJS_INTF_METHOD fstat(tTJSVariant *result,
										   tjs_int numparams,
										   tTJSVariant **param,
										   iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;

		ttstr filename = TVPGetPlacedPath(*param[0]);
		if (filename.length()) {
			if (!wcschr(filename.c_str(), '>')) {
				// 実ファイルが存在する場合
				TVPGetLocalName(filename);
				HANDLE hFile;
				if ((hFile = CreateFile(filename.c_str(), GENERIC_READ, 0, NULL ,
										OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL)) != INVALID_HANDLE_VALUE) {
					tTJSVariant size;
					tTJSVariant mtime;
					tTJSVariant ctime;
					tTJSVariant atime;

					LARGE_INTEGER fsize;
					if (GetFileSizeEx(hFile, &fsize)) {
						size = (tjs_int64)fsize.QuadPart;
					}
					FILETIME c, a, m;
					if (GetFileTime(hFile , &c, &a, &m)) {
						storeDate(atime, a, objthis);
						storeDate(ctime, c, objthis);
						storeDate(mtime, m, objthis);
					}

					if (result) {
						iTJSDispatch2 *dict;
						if ((dict = TJSCreateDictionaryObject()) != NULL) {
							dict->PropSet(TJS_MEMBERENSURE, L"size",  NULL, &size, dict);
							dict->PropSet(TJS_MEMBERENSURE, L"mtime", NULL, &mtime, dict);
							dict->PropSet(TJS_MEMBERENSURE, L"ctime", NULL, &ctime, dict);
							dict->PropSet(TJS_MEMBERENSURE, L"atime", NULL, &atime, dict);
							*result = dict;
							dict->Release();
						}
					}
					CloseHandle(hFile);
					return TJS_S_OK;
				}
			} else {
				IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
				if (in) {
					STATSTG stat;
					in->Stat(&stat, STATFLAG_NONAME);
					tTJSVariant size((tjs_int64)stat.cbSize.QuadPart);
					if (result) {
						iTJSDispatch2 *dict;
						if ((dict = TJSCreateDictionaryObject()) != NULL) {
							dict->PropSet(TJS_MEMBERENSURE, L"size",  NULL, &size, dict);
							*result = dict;
							dict->Release();
						}
					}
					in->Release();
					return TJS_S_OK;
				}
			}
		}

		TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + param[0]->GetString()).c_str());
		return TJS_S_OK;
	}

	void exportFile(const char *src, const char *dest) {
		ttstr filename = src;
		IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
		if (in) {
			ttstr storename = dest;
			IStream *out = TVPCreateIStream(storename, TJS_BS_WRITE);
			if (out) {
				BYTE buffer[1024*16];
				DWORD size;
				while (in->Read(buffer, sizeof buffer, &size) == S_OK && size > 0) {			
					out->Write(buffer, size, &size);
				}
				out->Release();
				in->Release();
			} else {
				in->Release();
				TVPThrowExceptionMessage((ttstr(TJS_W("cannot open storefile: ")) + storename).c_str());
			}
		} else {
			TVPThrowExceptionMessage((ttstr(TJS_W("cannot open readfile: ")) + filename).c_str());
		}
	}
	
};

NCB_ATTACH_CLASS(StoragesFstat, Storages) {
	RawCallback("fstat", &StoragesFstat::fstat, TJS_STATICMEMBER);
	NCB_METHOD(exportFile);
};

/**
 * 登録処理後
 */
static void PostRegistCallback()
{
	tTJSVariant var;
	TVPExecuteExpression(TJS_W("Date"), &var);
	dateClass = var.AsObject();
	TVPExecuteExpression(TJS_W("Date.setTime"), &var);
	dateSetTime = var.AsObject();
}

#define RELEASE(name) name->Release();name= NULL

/**
 * 開放処理前
 */
static void PreUnregistCallback()
{
	RELEASE(dateClass);
	RELEASE(dateSetTime);
}

NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);
