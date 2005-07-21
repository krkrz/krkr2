#include <windows.h>
#include <activscp.h>
#include <DispEx.h>
#include "tp_stub.h"
#include <stdio.h>

#pragma warning(disable: 4786)
#include <map>
using namespace std;

#define GLOBAL L"kirikiri"

/**
 * ログ出力用
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
	TVPAddLog(msg);
	va_end(args);
}

//---------------------------------------------------------------------------

#include "IDispatchWrapper.hpp"

//---------------------------------------------------------------------------

/*
 * Windows Script Host 処理用ネイティブインスタンス
 */
class NI_ScriptHost : public tTJSNativeInstance, IActiveScriptSite
{
protected:

	/// tjs global 保持用
	IDispatchEx *global;

	// ------------------------------------------------------
	// IUnknown 実装
	// ------------------------------------------------------
protected:
	ULONG refCount;
public:
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject) {
		*ppvObject = NULL;
		return E_NOTIMPL;
	}
	virtual ULONG _stdcall AddRef(void) {
		return ++refCount;
	}
	virtual ULONG _stdcall Release(void) {
		if(--refCount <= 0) return 0;
		return refCount;
	}

	// ------------------------------------------------------
	// IActiveScriptSite 実装
	// ------------------------------------------------------
public:
	virtual HRESULT __stdcall GetLCID(LCID *plcid) {
		return S_OK;
	}

	virtual HRESULT __stdcall GetItemInfo(LPCOLESTR pstrName,
										  DWORD dwReturnMask, IUnknown **ppunkItem, ITypeInfo **ppti) {
		if (ppti) {
			*ppti = NULL;
		}
		if (ppunkItem) {
			*ppunkItem = NULL;
			if (dwReturnMask & SCRIPTINFO_IUNKNOWN) {
				if (!_wcsicmp(GLOBAL, pstrName)) {
					global->AddRef();
					*ppunkItem = global;
				}
			}
		}
		return S_OK;
	}
	
	virtual HRESULT __stdcall GetDocVersionString(BSTR *pbstrVersion) {
		return S_OK;
	}
	
	virtual HRESULT __stdcall OnScriptTerminate(const VARIANT *pvarResult, const EXCEPINFO *ei) {
		return S_OK;
	}
	
	virtual HRESULT __stdcall OnStateChange(SCRIPTSTATE ssScriptState) {
		return S_OK;
	}
	
	virtual HRESULT __stdcall OnScriptError(IActiveScriptError *pscriptError) {
		log(TJS_W("OnScriptError"));
		ttstr errMsg;
		BSTR sourceLine;
		if (pscriptError->GetSourceLineText(&sourceLine) == S_OK) {
			log(TJS_W("source:%ls"), sourceLine);
			::SysFreeString(sourceLine);
		}
		DWORD sourceContext;
		ULONG lineNumber;
		LONG charPosition;
		if (pscriptError->GetSourcePosition(
			&sourceContext,
			&lineNumber,
			&charPosition) == S_OK) {
			log(TJS_W("context:%ld lineNo:%d pos:%d"), sourceContext, lineNumber, charPosition);
		}		
		EXCEPINFO ei;
		memset(&ei, 0, sizeof ei);
		if (pscriptError->GetExceptionInfo(&ei) == S_OK) {
			log(TJS_W("exception code:%x desc:%ls"), ei.wCode, ei.bstrDescription);
		}
		return S_OK;
	}

	virtual HRESULT __stdcall OnEnterScript(void) {
		return S_OK;
	}
	
	virtual HRESULT __stdcall OnLeaveScript(void) {
		return S_OK;
	}

	// ------------------------------------------------------
	// 処理部
	// ------------------------------------------------------

protected:
	/// 拡張子とProgId のマッピング
	map<ttstr, ttstr> extMap;
	// CLSID 比較用
	struct CompareCLSID : public binary_function<CLSID,CLSID,bool> {
		bool operator() (const CLSID &key1, const CLSID &key2) const {
			return (key1.Data1 < key2.Data1 ||
					key1.Data2 < key2.Data2 ||
					key1.Data3 < key2.Data3 ||
					key1.Data4[0] < key2.Data4[0] ||
					key1.Data4[1] < key2.Data4[1] ||
					key1.Data4[2] < key2.Data4[2] ||
					key1.Data4[3] < key2.Data4[3] ||
					key1.Data4[4] < key2.Data4[4] ||
					key1.Data4[5] < key2.Data4[5] ||
					key1.Data4[6] < key2.Data4[6] ||
					key1.Data4[7] < key2.Data4[7]);
		}
	};
	map<CLSID, IActiveScript*, CompareCLSID> scriptMap;

	/**
	 * 指定された ActiveScript エンジンを取得する
	 * @param type 拡張子 または progId または CLSID
	 * @return エンジンインターフェース
	 */
	IActiveScript *getScript(const tjs_char *type) {
		HRESULT hr;
		CLSID   clsid;
		
		// ProgId または CLSID の文字列表現からエンジンの CLSID を決定する
		OLECHAR *oleType = ::SysAllocString(type);
		if (FAILED(hr = CLSIDFromProgID(oleType, &clsid))) {
			hr = CLSIDFromString(oleType, &clsid);
		}
		::SysFreeString(oleType);

		if (SUCCEEDED(hr)) {
			map<CLSID, IActiveScript*, CompareCLSID>::const_iterator n = scriptMap.find(clsid);
			if (n != scriptMap.end()) {
				// すでに取得済みのエンジンの場合はそれを返す
				return n->second;
			} else {
				// 新規取得
				IActiveScript *pScript;
				hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IActiveScript, (void**)&pScript);
				if (SUCCEEDED(hr)) {
					IActiveScriptParse *pScriptParse;
					if (SUCCEEDED(pScript->QueryInterface(IID_IActiveScriptParse, (void **)&pScriptParse))) {
						// ActiveScriptSite を登録
						pScript->SetScriptSite(this);
						// グローバル変数の名前を登録
						pScript->AddNamedItem(GLOBAL, SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISSOURCE);
						// 初期化
						pScriptParse->InitNew();
						pScriptParse->Release();
						scriptMap[clsid] = pScript;
						return pScript;
					} else {
						log(TJS_W("QueryInterface IActipveScriptParse failed %s"), type);
						pScript->Release();
					}
				} else {
					log(TJS_W("CoCreateInstance failed %s"), type);
				}
			}
		} else {
			log(TJS_W("bad ProgId/CLSID %s"), type);
		}
		return NULL;
	}
	
public:
	/**
	 * コンストラクタ
	 */
	NI_ScriptHost() {
		// global の取得
		iTJSDispatch2 * tjsGlobal = TVPGetScriptDispatch();
		global = new IDispatchWrapper(tjsGlobal);
		tjsGlobal->Release();
		// 拡張子に対する ProgId のマッピングの登録
		extMap["js"]  = "JScript";
		extMap["vbs"] = "VBScript";
		extMap["pl"]  = "PerlScript";
		extMap["rb"]  = "RubyScript";
	}

	/**
	 * デストラクタ
	 */
	~NI_ScriptHost() {
		// エンジンの開放
 		map<CLSID, IActiveScript*, CompareCLSID>::iterator i = scriptMap.begin();
		while (i != scriptMap.end()) {
			i->second->Close();
			i->second->Release();
			i = scriptMap.erase(i);
		}
		// global を開放
		global->Release();
	}

	/**
	 * 拡張子を ProgId に変換する
	 * @param exe 拡張子
	 * @return ProgId
	 */
	const tjs_char *getProgId(const tjs_char *ext) {
		ttstr extStr(ext);
		extStr.ToLowerCase();
		map<ttstr, ttstr>::const_iterator n = extMap.find(extStr);
		if (n != extMap.end()) {
			return n->second.c_str();
		}
		return NULL;
	}

	/**
	 * 拡張子と ProgId の組を追加登録する
	 * @param exe 拡張子
	 * @param progId ProgId
	 */
	void addProgId(const tjs_char *ext, const tjs_char *progId) {
		ttstr extStr(ext);
		extStr.ToLowerCase();
		extMap[extStr] = progId;
	}

	/**
	 * スクリプトの実行
	 * @param progId スクリプトの種別
	 * @param script スクリプト本体
	 * @param result 結果格納先
	 */
	tjs_error exec(const tjs_char *progId, const tjs_char *script, tTJSVariant *result) {
		IActiveScript *pScript = getScript(progId);
		if (pScript) {
			IActiveScriptParse *pScriptParse;
			if (SUCCEEDED(pScript->QueryInterface(IID_IActiveScriptParse, (void **)&pScriptParse))) {
				
				// 結果格納用
				HRESULT hr;
				EXCEPINFO ei;
				VARIANT rs;
				memset(&ei, 0, sizeof ei);

				BSTR pParseText = ::SysAllocString(script);
				if (SUCCEEDED(hr = pScriptParse->ParseScriptText(pParseText, GLOBAL, NULL, NULL, 0, 0, 0L, &rs, &ei))) {
					hr = pScript->SetScriptState(SCRIPTSTATE_CONNECTED);
				}
				::SysFreeString(pParseText);
				
				switch (hr) {
				case S_OK:
					if (result) {
						IDispatchWrapper::storeVariant(*result, rs);
					}
					return TJS_S_OK;
				case DISP_E_EXCEPTION:
					log(TJS_W("exception code:%x desc:%ls"), ei.wCode, ei.bstrDescription);
					TVPThrowExceptionMessage(TJS_W("exception"));
					break;
				case E_POINTER:
					TVPThrowExceptionMessage(TJS_W("memory error"));
					break;
				case E_INVALIDARG:
					return TJS_E_INVALIDPARAM;
				case E_NOTIMPL:
					return TJS_E_NOTIMPL;
				case E_UNEXPECTED:
					return TJS_E_ACCESSDENYED;
				default:
					log(TJS_W("error:%x"), hr);
					return TJS_E_FAIL;
				}
			}
		}
		return TJS_E_FAIL;
	}

	/**
	 * スクリプトの実行
	 * @param progId スクリプトの種別
	 * @param script スクリプト本体
	 * @param result 結果格納先
	 */
	tjs_error execStorage(const tjs_char *progId, const tjs_char *filename, tTJSVariant *result) {
		
		tTJSTextReadStream * stream = TVPCreateTextStreamForRead(filename, TJS_W(""));
		try {
			tjs_error ret;
			ttstr data;
			stream->Read(data, 0);
			ret = exec(progId, data.c_str(), result);
		}
		catch(...)
		{
			stream->Destruct();
			throw;
		}
		stream->Destuct();
		return ret;
	}
};

// クラスID
static tjs_int32 ClassID_ScriptHost = -1;

/**
 * Windows Script Host 実行処理用メソッド
 * exec(script [,lang])
 * 引数が２つ以上ある場合は、２つ目を言語指定とみなしてスクリプトを実行する。
 * そうでない場合は tjs のスクリプトとみなして実行する。
 */
class tExecFunction : public tTJSDispatch
{
protected:
	/// 元のメソッド
	iTJSDispatch2 *originalFunction;
public:	
	/// コンストラクタ
	tExecFunction(tTJSVariant &original) {
		originalFunction = original.AsObject();
	}
	/// デストラクタ
	~tExecFunction() {
		originalFunction->Release();
	}
	
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		// ネイティブオブジェクトの取得
		if (!objthis) return TJS_E_NATIVECLASSCRASH;
		NI_ScriptHost *_this;
		tjs_error hr;
		hr = objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
											ClassID_ScriptHost, (iTJSNativeInstance**)&_this); \
		if (TJS_FAILED(hr)) return TJS_E_NATIVECLASSCRASH;
		
		if (membername) return TJS_E_MEMBERNOTFOUND;
		if (numparams == 0) return TJS_E_BADPARAMCOUNT;

		if (numparams > 1) {
			return _this->exec(param[1]->GetString(), param[0]->GetString(), result);
		} else {
			return originalFunction->FuncCall(flag, membername, hint, result, numparams, param, objthis);
		}
	}
};

/**
 * Windows Script Host 実行処理用メソッド
 * execStorage(storage [,lang])
 * 引数が２つ以上ある場合は、２つ目を言語指定とみなしてストレージを実行する。
 * そうでない場合は、storage の拡張子を調べ、該当する言語が存在すればその言語で、
 * 存在しなければ tjs のストレージとみなして実行する。
 */
class tExecStorageFunction : public tTJSDispatch
{
protected:
	/// 元のメソッド
	iTJSDispatch2 *originalFunction;
public:	
	/// コンストラクタ
	tExecStorageFunction(tTJSVariant &original) {
		originalFunction = original.AsObject();
	}
	/// デストラクタ
	~tExecStorageFunction() {
		originalFunction->Release();
	}

	const tjs_char *getProgId(NI_ScriptHost *_this, tjs_int numparams, tTJSVariant **param) {
		if (numparams > 1) {
			return param[1]->GetString();
		} else {
			const tjs_char *ext = wcsrchr(param[0]->GetString(), '.');
			if (ext) {
				return _this->getProgId(ext + 1);
			}
		}
		return NULL;
	}
	
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		// ネイティブオブジェクトの取得
		if (!objthis) return TJS_E_NATIVECLASSCRASH;
		NI_ScriptHost *_this;
		tjs_error hr;
		hr = objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
											ClassID_ScriptHost, (iTJSNativeInstance**)&_this); \
		if (TJS_FAILED(hr)) return TJS_E_NATIVECLASSCRASH;
		
		if (membername) return TJS_E_MEMBERNOTFOUND;
		if (numparams == 0) return TJS_E_BADPARAMCOUNT;

		const tjs_char *progId;
		if ((progId = getProgId(_this, numparams, param))) {
			return _this->execStorage(progId, param[0]->GetString(), result);
		} else {
			return originalFunction->FuncCall(flag, membername, hint, result, numparams, param, objthis);
		}
	}
	
};


//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

extern void registArchive();
extern void unregistArchive();

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
static BOOL gOLEInitialized = false;

extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	{
		// グローバル
		iTJSDispatch2 * global = TVPGetScriptDispatch();

		// Scripts オブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Scripts"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {

			// クラスオブジェクト生成
			ClassID_ScriptHost = TJSRegisterNativeClass(TJS_W("ScriptHost"));
			iTJSNativeInstance *ni = new NI_ScriptHost();
			dispatch->NativeInstanceSupport(TJS_NIS_REGISTER, ClassID_ScriptHost, &ni);

			// 処理用メソッドを乗っ取る

			// exec
			{
				const tjs_char *memberName = TJS_W("exec");
				tTJSVariant var;
				dispatch->PropGet(0, memberName, NULL, &var, dispatch);
				tTJSDispatch *method = new tExecFunction(var);
				var = tTJSVariant(method, dispatch);
				method->Release();
				dispatch->PropSet(
					TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
					memberName, // メンバ名 ( かならず TJS_W( ) で囲む )
					NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
					&var, // 登録する値
					dispatch // コンテキスト
					);
			}

			// execStorage
			{
				const tjs_char *memberName = TJS_W("execStorage");
				tTJSVariant var;
				dispatch->PropGet(0, memberName, NULL, &var, dispatch);
				tTJSDispatch *method = new tExecStorageFunction(var);
				var = tTJSVariant(method, dispatch);
				method->Release();
				dispatch->PropSet(
					TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
					memberName, // メンバ名 ( かならず TJS_W( ) で囲む )
					NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
					&var, // 登録する値
					dispatch // コンテキスト
					);
			}
		}
		global->Release();
	}
	
	// この時点での TVPPluginGlobalRefCount の値を
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
	// 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
	// 解放時にはこれと同じか、これよりも少なくなってないとならない。
	// そうなってなければ、どこか別のところで関数などが参照されていて、
	// プラグインは解放できないと言うことになる。
	
	if (!gOLEInitialized) {
		if (SUCCEEDED(OleInitialize(NULL))) {
			gOLEInitialized = true;
		} else {
			log(TJS_W("OLE 初期化失敗"));
		}
	}
	
	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数。

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
	// E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す

	if (gOLEInitialized) {
		OleUninitialize();
		gOLEInitialized = false;
	}
	
	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();
	
	return S_OK;
}
