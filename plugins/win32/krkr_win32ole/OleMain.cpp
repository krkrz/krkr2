#include <windows.h>
#include <DispEx.h>
#include "tp_stub.h"
#include <stdio.h>

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

#if 0
/**
 * OLE -> 吉里吉里 イベントディスパッチャ
 * sender (IUnknown) から DIID のイベントを受理し、
 * receiver (tTJSDispatch2) に送信する。
 */ 
class SuperSink : public IDispatch
{
protected:
	REFIID diid;
	//CComPtr<IUnkown> sender;
	DWORD cookie;
	
	VNBSink(IUnkown *sender, GUID diid) {

		// advise する
	}

	//----------------------------------------------------------------------------
	// IUnknown 実装
	
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
											 void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (riid == IID_IUnknown ||
			riid == IID_IDispatch||
			riid == diid) {
			if (ppvObject == NULL)
				return E_POINTER;
			*ppvObject = this;
			AddRef();
			return S_OK;
		} else {
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return 1;
	}

	ULONG STDMETHODCALLTYPE Release() {
		return 1;
	}
	
	// -------------------------------------
	// IDispatch の実装
public:
	STDMETHOD (GetTypeInfoCount) (UINT* pctinfo)
	{
		return	E_NOTIMPL;
	}

	STDMETHOD (GetTypeInfo) (UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
	{
		return	E_NOTIMPL;
	}

	STDMETHOD (GetIDsOfNames) (REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid)
	{
		return	E_NOTIMPL;
	}

	STDMETHOD (Invoke) (DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr)
	{
		// XXX sender へのイベント送信処理
		return S_OK;
	}
};
#endif

#include "IDispatchWrapper.hpp"

//---------------------------------------------------------------------------

/*
 * WIN32OLE ネイティブインスタンス
 */
class NI_WIN32OLE : public tTJSNativeInstance // ネイティブインスタンス
{
public:
	IDispatch *pDispatch;

	NI_WIN32OLE() {
		// コンストラクタ
		pDispatch = NULL;
	}

	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		// TJS2 オブジェクトが作成されるときに呼ばれる
		if (numparams >= 1) {
			if (param[0]->Type() == tvtString) {
				const tjs_char *name = param[0]->GetString();
				HRESULT hr;
				CLSID   clsid;
				OLECHAR *oleName = SysAllocString(name);
				if (FAILED(hr = CLSIDFromProgID(oleName, &clsid))) {
					hr = CLSIDFromString(oleName, &clsid);
				}
				SysFreeString(oleName);
				if (SUCCEEDED(hr)) {
					// COM 接続してIDispatch を取得する
					/* get IDispatch interface */
					hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&pDispatch);
					if (SUCCEEDED(hr)) {
						// missing メソッドの登録
						tTJSVariant name(TJS_W("missing"));
						tjs_obj->ClassInstanceInfo(TJS_CII_SET_MISSING, 0, &name);
						return TJS_S_OK;
					} else {
						log(TJS_W("CoCreateInstance failed %s"), name);
					}
				} else {
					log(TJS_W("bad CLSID %s"), name);
				}
			} 
			return TJS_E_INVALIDPARAM;
		} else {
			return TJS_E_BADPARAMCOUNT;
		}
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// オブジェクトが無効化されるときに呼ばれる
		if (pDispatch) {
			pDispatch->Release();
		}
	}

	/**
	 * メソッド実行
	 */
	tjs_error invoke(DWORD wFlags,
					 const tjs_char *membername,
					 tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param) {
		if (pDispatch) {
			return iTJSDispatch2Wrapper::Invoke(pDispatch,
												wFlags,
												membername,
												result,
												numparams,
												param);
		}
		return TJS_E_FAIL;
	}
	
	/**
	 * メソッド実行
	 */
	tjs_error invoke(DWORD wFlags,
					 tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param) {
		//log(TJS_W("native invoke %d"), numparams);
		if (pDispatch) {
			// パラメータの１つ目がメソッド名
			if (numparams > 0) {
				if (param[0]->Type() == tvtString) {
					return iTJSDispatch2Wrapper::Invoke(pDispatch,
														wFlags,
														param[0]->GetString(),
														result,
														numparams - 1,
														param ? param + 1 : NULL);
				} else {
					return TJS_E_INVALIDPARAM;
				}
			} else {
				return TJS_E_BADPARAMCOUNT;
			}
		}
		return TJS_E_FAIL;
	}

	/**
	 * 定数の取得
	 */
protected:

	/**
	 * 定数の取得
	 * @param pTypeInfo TYPEINFO
	 * @param target 格納先
	 */
	void getConstant(ITypeInfo *pTypeInfo, iTJSDispatch2 *target) {
		// 個数情報
		TYPEATTR  *pTypeAttr = NULL;
		if (SUCCEEDED(pTypeInfo->GetTypeAttr(&pTypeAttr))) {
			for (int i=0; i<pTypeAttr->cVars; i++) {
				VARDESC *pVarDesc = NULL;
				if (SUCCEEDED(pTypeInfo->GetVarDesc(i, &pVarDesc))) {
					if (pVarDesc->varkind == VAR_CONST &&
						!(pVarDesc->wVarFlags & (VARFLAG_FHIDDEN | VARFLAG_FRESTRICTED | VARFLAG_FNONBROWSABLE))) {
						BSTR bstr = NULL;
						unsigned int len;
						if (SUCCEEDED(pTypeInfo->GetNames(pVarDesc->memid, &bstr, 1, &len)) && len >= 0 && bstr) {
							log(TJS_W("const:%s"), bstr);
							tTJSVariant result;
							IDispatchWrapper::storeVariant(result, *(pVarDesc->lpvarValue));
							target->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP,
											bstr,
											NULL,
											&result,
											target
											);
							SysFreeString(bstr);
						}
					}
					pTypeInfo->ReleaseVarDesc(pVarDesc);
				}
			}
			pTypeInfo->ReleaseTypeAttr(pTypeAttr);
		}
	}

	/**
	 * 定数の取得
	 * @param pTypeLib TYPELIB
	 * @param target 格納先
	 */
	void getConstant(ITypeLib *pTypeLib, iTJSDispatch2 *target) {
		unsigned int count = pTypeLib->GetTypeInfoCount();
		for (unsigned int i=0; i<count; i++) {
			ITypeInfo *pTypeInfo = NULL;
			if (SUCCEEDED(pTypeLib->GetTypeInfo(i, &pTypeInfo))) {
				getConstant(pTypeInfo, target);
				pTypeInfo->Release();
			}
		}
	}

public:
	/**
	 * 定数の取得
	 * @param target 格納先
	 */
	void getConstant(iTJSDispatch2 *target) {
		if (target) {
			ITypeInfo *pTypeInfo = NULL;
			if (SUCCEEDED(pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
				unsigned int index = 0;
				ITypeLib *pTypeLib = NULL;
				if (SUCCEEDED(pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
					getConstant(pTypeLib, target);
					pTypeLib->Release();
				}
				pTypeInfo->Release();
			}
		}
	}

};
//---------------------------------------------------------------------------
/*
	これは NI_WIN32OLE のオブジェクトを作成して返すだけの関数です。
	後述の TJSCreateNativeClassForPlugin の引数として渡します。
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_WIN32OLE()
{
	return new NI_WIN32OLE();
}
//---------------------------------------------------------------------------
/*
	TJS2 のネイティブクラスは一意な ID で区別されている必要があります。
	これは後述の TJS_BEGIN_NATIVE_MEMBERS マクロで自動的に取得されますが、
	その ID を格納する変数名と、その変数をここで宣言します。
	初期値には無効な ID を表す -1 を指定してください。
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_WIN32OLE
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 用の「クラス」を作成して返す関数です。
*/
static iTJSDispatch2 * Create_NC_WIN32OLE()
{
	// クラスオブジェクト
	tTJSNativeClassForPlugin * classobj = TJSCreateNativeClassForPlugin(TJS_W("WIN32OLE"), Create_NI_WIN32OLE);

	// ネイティブメンバー
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/WIN32OLE)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_WIN32OLE,
			/*TJS class name*/WIN32OLE)
		{
			// NI_WIN32OLE::Construct にも内容を記述できるので
			// ここでは何もしない
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/WIN32OLE)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/invoke) // invoke メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/invoke)
			
		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/set) // set メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->invoke(DISPATCH_PROPERTYPUT, result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/set)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/get) // get メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->invoke(DISPATCH_PROPERTYGET, result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/get)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/missing) // missing メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			if (numparams < 3) {return TJS_E_BADPARAMCOUNT;};
			bool ret = false;
			const tjs_char *membername = param[1]->GetString();
			if ((int)*param[0]) {
				// put
				ret = TJS_SUCCEEDED(_this->invoke(DISPATCH_PROPERTYPUT, membername, NULL, 1, &param[2]));
			} else {
				// get
				tTJSVariant result;
				if ((ret = TJS_SUCCEEDED(_this->invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, membername, &result, 0, NULL)))) {
					iTJSDispatch2 *value = param[2]->AsObject();
					if (value) {
						value->PropSet(0, NULL, NULL, &result, NULL);
						value->Release();
					}
				}
			}
			if (result) {
				*result = ret;
			}
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/missing)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getConstant) // getConstant メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			if (numparams > 0) {
				iTJSDispatch2 *store = param[0]->AsObject();
				if (store) {
					_this->getConstant(store);
					store->Release();
				}
			} else {
				_this->getConstant(objthis);
			}
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/getConstant)
			
	TJS_END_NATIVE_MEMBERS

	/*
		この関数は classobj を返します。
	*/
	return classobj;
}
//---------------------------------------------------------------------------
/*
	TJS_NATIVE_CLASSID_NAME は一応 undef しておいたほうがよいでしょう
*/
#undef TJS_NATIVE_CLASSID_NAME
//---------------------------------------------------------------------------

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

	tTJSVariant val;

	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	{
		//-----------------------------------------------------------------------
		// 1 まずクラスオブジェクトを作成
		iTJSDispatch2 * tjsclass = Create_NC_WIN32OLE();
		
		// 2 tjsclass を tTJSVariant 型に変換
		val = tTJSVariant(tjsclass);
		
		// 3 すでに val が tjsclass を保持しているので、tjsclass は
		//   Release する
		tjsclass->Release();
		
		
		// 4 global の PropSet メソッドを用い、オブジェクトを登録する
		global->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("WIN32OLE"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&val, // 登録する値
			global // コンテキスト ( global でよい )
			);
		//-----------------------------------------------------------------------
	}

	// - global を Release する
	global->Release();

	// もし、登録する関数が複数ある場合は 1 ～ 4 を繰り返す

	// val をクリアする。
	// これは必ず行う。そうしないと val が保持しているオブジェクト
	// が Release されず、次に使う TVPPluginGlobalRefCount が正確にならない。
	val.Clear();

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

	/*
		ただし、クラスの場合、厳密に「オブジェクトが使用中である」ということを
		知るすべがありません。基本的には、Plugins.unlink によるプラグインの解放は
		危険であると考えてください (いったん Plugins.link でリンクしたら、最後ま
		でプラグインを解放せず、プログラム終了と同時に自動的に解放させるのが吉)。
	*/

	// TJS のグローバルオブジェクトに登録した WIN32OLE クラスなどを削除する

	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// - global の DeleteMember メソッドを用い、オブジェクトを削除する
	if(global)
	{
		// TJS 自体が既に解放されていたときなどは
		// global は NULL になり得るので global が NULL でない
		// ことをチェックする

		global->DeleteMember(
			0, // フラグ ( 0 でよい )
			TJS_W("WIN32OLE"), // メンバ名
			NULL, // ヒント
			global // コンテキスト
			);
	}
	
	// - global を Release する
	if(global) global->Release();

	if (gOLEInitialized) {
		OleUninitialize();
		gOLEInitialized = false;
	}
	
	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------

