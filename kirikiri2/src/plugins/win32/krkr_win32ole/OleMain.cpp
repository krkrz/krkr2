#include <windows.h>
#include <DispEx.h>
#include "tp_stub.h"
#include <stdio.h>

// ATL
#if _MSC_VER == 1200
// Microsoft SDK のものとかちあうので排除
#define __IHTMLControlElement_INTERFACE_DEFINED__
#endif

#include <atlbase.h>
static CComModule _Module;
#include <atlwin.h>
#include <atlcom.h>
#include <atliface.h>
#define _ATL_DLL
#include <atlhost.h>
#include <ExDispID.h>


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

/**
 * OLE -> 吉里吉里 イベントディスパッチャ
 * sender (IUnknown) から DIID のイベントを受理し、
 * receiver (tTJSDispatch2) に送信する。
 */ 
class EventSink : public IDispatch
{
protected:
	int refCount;
	REFIID diid;
	ITypeInfo *pTypeInfo;
	iTJSDispatch2 *receiver;

public:
	EventSink(GUID diid, ITypeInfo *pTypeInfo, iTJSDispatch2 *receiver) : diid(diid), pTypeInfo(pTypeInfo), receiver(receiver) {
		refCount = 1;
		if (pTypeInfo) {
			pTypeInfo->AddRef();
		}
		if (receiver) {
			receiver->AddRef();
		}
	}

	~EventSink() {
		if (receiver) {
			receiver->Release();
		}
		if (pTypeInfo) {
			pTypeInfo->Release();
		}
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
		refCount++;
		return refCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		int ret = --refCount;
		if (ret <= 0) {
			delete this;
			ret = 0;
		}
		return ret;
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

	STDMETHOD (Invoke) (DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr)
	{
		BSTR bstr = NULL;
		if (pTypeInfo) {
			unsigned int len;
			pTypeInfo->GetNames(dispid, &bstr, 1, &len);
		}
		HRESULT hr = IDispatchWrapper::InvokeEx(receiver, bstr, wFlags, pdispparams, pvarResult, pexcepinfo);
		if (hr == DISP_E_MEMBERNOTFOUND) {
			//log(L"member not found:%ws", bstr);
			hr = S_OK;
		}
		if (bstr) {
			SysFreeString(bstr);
		}
		return hr;
	}
};

//---------------------------------------------------------------------------

/*
 * WIN32OLE ネイティブインスタンス
 */
class NI_WIN32OLE : public tTJSNativeInstance // ネイティブインスタンス
{
public:
	IDispatch *pDispatch;

protected:
	struct EventInfo {
		IID diid;
		DWORD cookie;
		EventInfo(REFIID diid, DWORD cookie) : diid(diid), cookie(cookie) {};
	};
	vector<EventInfo> events;

	void clearEvent() {
		if (pDispatch) {
			vector<EventInfo>::iterator i = events.begin();
			while (i != events.end()) {
				AtlUnadvise(pDispatch, i->diid, i->cookie);
				i++;
			}
			events.clear();
		}
	}

public:
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
						log(L"CoCreateInstance failed %ws", name);
					}
				} else {
					log(L"bad CLSID %ws", name);
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
		clearEvent();
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
		//log(L"native invoke %d", numparams);
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
	 * メソッド実行
	 */
	tjs_error missing(tTJSVariant *result, tjs_int numparams, tTJSVariant **param) {

		if (numparams < 3) {return TJS_E_BADPARAMCOUNT;};
		bool ret = false;
		const tjs_char *membername = param[1]->GetString();
		if ((int)*param[0]) {
			// put
			ret = TJS_SUCCEEDED(invoke(DISPATCH_PROPERTYPUT, membername, NULL, 1, &param[2]));
		} else {
			// get
			tTJSVariant result;
			tjs_error err;
			ret = TJS_SUCCEEDED(err = invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, membername, &result, 0, NULL));
			if (err == TJS_E_BADPARAMCOUNT) {
				result = new iTJSDispatch2WrapperForMethod(pDispatch, membername);
				ret = true;
			}
			if (ret) {
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


protected:

	/**
	 * デフォルトの IID を探す
	 * @param pitf 名前
	 * @param piid 取得したIIDの格納先
	 * @param ppTypeInfo 関連する方情報
	 */
	
	HRESULT findDefaultIID(IID *piid, ITypeInfo **ppTypeInfo) {

		HRESULT hr;

		IProvideClassInfo2 *pProvideClassInfo2;
		hr = pDispatch->QueryInterface(IID_IProvideClassInfo2, (void**)&pProvideClassInfo2);
		if (SUCCEEDED(hr)) {
			hr = pProvideClassInfo2->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID, piid);
			pProvideClassInfo2->Release();
			ITypeInfo *pTypeInfo;
			if (SUCCEEDED(hr = pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
				ITypeLib *pTypeLib;
				unsigned int index;
				if (SUCCEEDED(hr = pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
					hr = pTypeLib->GetTypeInfoOfGuid(*piid, ppTypeInfo);
				}
			}
			return hr;
		}

		IProvideClassInfo *pProvideClassInfo;
		if (SUCCEEDED(hr = pDispatch->QueryInterface(IID_IProvideClassInfo, (void**)&pProvideClassInfo))) {
			ITypeInfo *pTypeInfo;
			if (SUCCEEDED(hr = pProvideClassInfo->GetClassInfo(&pTypeInfo))) {
				
				TYPEATTR *pTypeAttr;
				if (SUCCEEDED(hr = pTypeInfo->GetTypeAttr(&pTypeAttr))) {
					int i;
					for (i = 0; i < pTypeAttr->cImplTypes; i++) {
						int iFlags;
						if (SUCCEEDED(hr = pTypeInfo->GetImplTypeFlags(i, &iFlags))) {
							if ((iFlags & IMPLTYPEFLAG_FDEFAULT) &&	(iFlags & IMPLTYPEFLAG_FSOURCE)) {
								HREFTYPE hRefType;
								if (SUCCEEDED(hr = pTypeInfo->GetRefTypeOfImplType(i, &hRefType))) {
									if (SUCCEEDED(hr = pTypeInfo->GetRefTypeInfo(hRefType, ppTypeInfo))) {
										break;
									}
								}
							}
						}
					}
					pTypeInfo->ReleaseTypeAttr(pTypeAttr);
				}
				pTypeInfo->Release();
			}
			pProvideClassInfo->Release();
		}

		if (!*ppTypeInfo) {
			if (SUCCEEDED(hr)) {
				hr = E_UNEXPECTED;
			}
		} else {
			TYPEATTR *pTypeAttr;
			hr = (*ppTypeInfo)->GetTypeAttr(&pTypeAttr);
			if (SUCCEEDED(hr)) {
				*piid = pTypeAttr->guid;
				(*ppTypeInfo)->ReleaseTypeAttr(pTypeAttr);
			} else {
				(*ppTypeInfo)->Release();
				*ppTypeInfo = NULL;
			}
		}
		return hr;
	}
	
	/**
	 * IID を探す
	 * @param pitf 名前
	 * @param piid 取得したIIDの格納先
	 * @param ppTypeInfo 関連する方情報
	 */
	HRESULT findIID(const tjs_char *pitf, IID *piid, ITypeInfo **ppTypeInfo) {

		if (pitf == NULL) {
			return findDefaultIID(piid, ppTypeInfo);
		}

		HRESULT hr;
		ITypeInfo *pTypeInfo;
		if (SUCCEEDED(hr = pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
			ITypeLib *pTypeLib;
			unsigned int index;
			if (SUCCEEDED(hr = pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
				bool found = false;
				unsigned int count = pTypeLib->GetTypeInfoCount();
				for (index = 0; index < count; index++) {
					ITypeInfo *pTypeInfo;
					if (SUCCEEDED(pTypeLib->GetTypeInfo(index, &pTypeInfo))) {
						TYPEATTR *pTypeAttr;
						if (SUCCEEDED(pTypeInfo->GetTypeAttr(&pTypeAttr))) {
							if (pTypeAttr->typekind == TKIND_COCLASS) {
								int type;
								for (type = 0; !found && type < pTypeAttr->cImplTypes; type++) {
									HREFTYPE RefType;
									if (SUCCEEDED(pTypeInfo->GetRefTypeOfImplType(type, &RefType))) {
										ITypeInfo *pImplTypeInfo;
										if (SUCCEEDED(pTypeInfo->GetRefTypeInfo(RefType, &pImplTypeInfo))) {
											BSTR bstr = NULL;
											if (SUCCEEDED(pImplTypeInfo->GetDocumentation(-1, &bstr, NULL, NULL, NULL))) {
												if (wcscmp(pitf, bstr) == 0) {
													TYPEATTR *pImplTypeAttr;
													if (SUCCEEDED(pImplTypeInfo->GetTypeAttr(&pImplTypeAttr))) {
														found = true;
														*piid = pImplTypeAttr->guid;
														if (ppTypeInfo) {
															*ppTypeInfo = pImplTypeInfo;
															(*ppTypeInfo)->AddRef();
														}
														pImplTypeInfo->ReleaseTypeAttr(pImplTypeAttr);
													}
												}
												SysFreeString(bstr);
											}
											pImplTypeInfo->Release();
										}
									}
								}
							}
							pTypeInfo->ReleaseTypeAttr(pTypeAttr);
						}
						pTypeInfo->Release();
					}
					if (found) {
						break;
					}
				}
				if (!found) {
					hr = E_NOINTERFACE;
				}
				pTypeLib->Release();
			}
			pTypeInfo->Release();
		}
		return hr;
	}

	bool addEvent(const tjs_char *diidName, iTJSDispatch2 *receiver) {
		bool ret = false;
		IID diid;
		ITypeInfo *pTypeInfo;
		if (SUCCEEDED(findIID(diidName, &diid, &pTypeInfo))) {
			EventSink *sink = new EventSink(diid, pTypeInfo, receiver);
			DWORD cookie;
			if (SUCCEEDED(AtlAdvise(pDispatch, sink, diid, &cookie))) {
				events.push_back(EventInfo(diid, cookie));
				ret = true;
			}
			sink->Release();
			if (pTypeInfo) {
				pTypeInfo->Release();
			}
		}
		return ret;
	}
	
public:

	tjs_error addEvent(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		const tjs_char *diidName = param[0]->GetString();
		bool success = false;
		if (numparams > 1) {
			iTJSDispatch2 *receiver = param[1]->AsObject();
			if (receiver) {
				success = addEvent(diidName, receiver);
				receiver->Release();
			}
		} else {
			success = addEvent(diidName, objthis);
		}
		if (!success) {
			log(L"イベント[%ws]の登録に失敗しました", diidName);
		}
		return TJS_S_OK;
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
							//log(L"const:%s", bstr);
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

	/**
	 * メソッド実行
	 */
	tjs_error getConstant(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams > 0) {
			iTJSDispatch2 *store = param[0]->AsObject();
			if (store) {
				getConstant(store);
				store->Release();
			}
		} else {
			getConstant(objthis);
		}
		return TJS_S_OK;
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
 *	TJS2 用の「クラス」を作成して返す関数です。
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
			return _this->missing(result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/missing)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getConstant) // getConstant メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->getConstant(numparams, param, objthis);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/getConstant)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/addEvent) // addEvent メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->addEvent(numparams, param, objthis);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/addEvent)
			
	TJS_END_NATIVE_MEMBERS

	return classobj;
}
//---------------------------------------------------------------------------
/*
	TJS_NATIVE_CLASSID_NAME は一応 undef しておいたほうがよいでしょう
*/
#undef TJS_NATIVE_CLASSID_NAME
//---------------------------------------------------------------------------


/**
 * DHTMLの外部呼出し(window.external)と各種UI処理を受理するためのインターフェースクラス。
 * それぞれクラス中の対応するメソッドを呼び出している。継承して中身を記述することで
 * 挙動が変更できる。パラメータはCOMスタイルのもの(VARIANT)になるので注意
 * XXX TJS のメソッドを呼び出しできるように後で改良予定。現在のものは「処理をしない」
 * ためのものになっている。
 */ 
class CExternalUI : public IDocHostUIHandlerDispatch {

protected:
	IDispatchEx *dispatchEx;
	
public:
	CExternalUI() {
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		dispatchEx = new IDispatchWrapper(global);
		global->Release();
	}

	~CExternalUI() {
		dispatchEx->Release();
	}

	//----------------------------------------------------------------------------
	// IUnknown 実装
	
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
											 void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (dispatchEx && (riid == IID_IUnknown || riid == IID_IDispatch || riid == IID_IDispatchEx)) {
			//log(L"get dispatchEx");
			if (ppvObject == NULL)
				return E_POINTER;
			dispatchEx->AddRef();
			*ppvObject = dispatchEx;
			return S_OK;
		} else if (riid == IID_IUnknown || riid == IID_IDispatch) {
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

	// XXX MSHTML からの呼び出しがおかしいので開放しないようにしている
	ULONG STDMETHODCALLTYPE AddRef() {
		return 1;
	}

	// XXX MSHTML からの呼び出しがおかしいので開放しないようにしている
	ULONG STDMETHODCALLTYPE Release() {
		return 1;
	}

	//----------------------------------------------------------------------------
	// IDispatch 実装

	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo) {
		return E_NOTIMPL;
	}

	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo) {
		return E_NOTIMPL;
	}

	/**
	 * メソッド名とIDの対応をとるメソッド
	 * regszNames にメソッド名の配列がくるので、
	 * rgdispid に対応する dispid を返してやる
	 */
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
							 LCID lcid, DISPID* rgdispid) {
		return E_NOTIMPL;
	}

	/**
	 * メソッド実行
	 * dispidMember でメソッドが指定される。引数は pdispparams に VARIANT の配列
	 * の形でわたされるのでそれを使う
	 */
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
					  LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
					  EXCEPINFO* pexcepinfo, UINT* puArgErr) {
		return E_NOTIMPL;
	}

	//----------------------------------------------------------------------------
	// IDocHostUIHandlerDispatch の実装

	
	/**
	 * コンテキストメニュー処理
	 * 何もしないことでメニューを消している
	 */
	HRESULT STDMETHODCALLTYPE ShowContextMenu( 
		/* [in] */ DWORD dwID,
		/* [in] */ DWORD x,
		/* [in] */ DWORD y,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pdispReserved,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal) {
		*dwRetVal = S_OK;      //This is what the WebBrowser control is looking for.
		//You can show your own context menu here.
		return S_OK;        
	}

	HRESULT STDMETHODCALLTYPE GetHostInfo( 
		/* [out][in] */ DWORD __RPC_FAR *pdwFlags,
		/* [out][in] */ DWORD __RPC_FAR *pdwDoubleClick) {
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ShowUI( 
		/* [in] */ DWORD dwID,
		/* [in] */ IUnknown __RPC_FAR *pActiveObject,
		/* [in] */ IUnknown __RPC_FAR *pCommandTarget,
		/* [in] */ IUnknown __RPC_FAR *pFrame,
		/* [in] */ IUnknown __RPC_FAR *pDoc,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE HideUI( void) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE UpdateUI( void) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ VARIANT_BOOL fEnable) {
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
		/* [in] */ VARIANT_BOOL fActivate) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
		/* [in] */ VARIANT_BOOL fActivate) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE ResizeBorder( 
		/* [in] */ long left,
		/* [in] */ long top,
		/* [in] */ long right,
		/* [in] */ long bottom,
		/* [in] */ IUnknown __RPC_FAR *pUIWindow,
		/* [in] */ VARIANT_BOOL fFrameWindow) {
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
		/* [in] */ DWORD hWnd,
		/* [in] */ DWORD nMessage,
		/* [in] */ DWORD wParam,
		/* [in] */ DWORD lParam,
		/* [in] */ BSTR bstrGuidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal) {
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
		/* [out] */ BSTR __RPC_FAR *pbstrKey,
		/* [in] */ DWORD dw) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE GetDropTarget( 
		/* [in] */ IUnknown __RPC_FAR *pDropTarget,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDropTarget) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE GetExternal( 
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) {
		*ppDispatch = this;
		return S_OK;
	}
        
	HRESULT STDMETHODCALLTYPE TranslateUrl( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ BSTR bstrURLIn,
		/* [out] */ BSTR __RPC_FAR *pbstrURLOut) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE FilterDataObject( 
		/* [in] */ IUnknown __RPC_FAR *pDO,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDORet) {
		return E_NOTIMPL;
	}
};

/*
 * ActiveX ネイティブインスタンス
 */
class NI_ActiveX : public NI_WIN32OLE, CWindowImpl<NI_ActiveX, CAxWindow>
{
protected:
	int left;
	int top;
	int width;
	int height;

protected:
	CExternalUI *externalUI;

public:
	NI_ActiveX() {
		externalUI = NULL;
	}

	~NI_ActiveX() {
		if (externalUI) {
			delete externalUI;
		}
	}
	
public:
	/**
	 * コンストラクタ
	 * @param name  識別名
	 * @param left  表示位置
	 * @param top   表示位置
	 * @param width  表示サイズ
	 * @param height 高さ指定
	 */
	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		left = 0;
		top  = 0;
		width  = 100; // デフォルト
		height = 100; // デフォルト
		
		// パラメータはひとつ必須
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		// 第一パラメータは文字列
		if (param[0]->Type() != tvtString) {
			return TJS_E_INVALIDPARAM;
		}
			
		const tjs_char *name = param[0]->GetString();
		OLECHAR *oleName = SysAllocString(name);

		// 外窓用
		HWND handle = 0;
		DWORD style = WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN;

		if (numparams >= 2) {
			// 第二パラメータから HWND を取得
			iTJSDispatch2 *win = param[1]->AsObjectNoAddRef();
			if (win) {
				tTJSVariant hwnd;
				if (win->PropGet(0, TJS_W("HWND"), NULL, &hwnd, win) == TJS_S_OK) {
					handle = (HWND)(int)hwnd;
					style = WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE; // 子ウインドウ
					RECT rect;
					::GetClientRect(handle, &rect);   // 親全領域
					left   = rect.left;
					top    = rect.top;
					width  = rect.right - rect.left;
					height = rect.bottom - rect.top;
				}
			}
			// 表示領域の変更
			if (numparams >= 6) {
				left   = (int)param[2];
				top    = (int)param[3];
				width  = (int)param[4];
				height = (int)param[5];
			}
		}

		RECT rect;
		rect.left   = left;
		rect.top    = top;
		rect.right  = left + width;
		rect.bottom = top  + height;
		
		tjs_error ret = TJS_E_INVALIDPARAM;
		HRESULT hr;

		// ウインドウ生成
		// ウインドウ生成
		if (Create(handle, rect, NULL, style)) {

			// コントロール生成
			hr = CreateControl(oleName);
			if (SUCCEEDED(hr)) {
				// IDispatch取得
				hr = QueryControl(IID_IDispatch, (void**)&pDispatch);
				if (SUCCEEDED(hr)) {
					// missing メソッドの登録
					tTJSVariant name(TJS_W("missing"));
					tjs_obj->ClassInstanceInfo(TJS_CII_SET_MISSING, 0, &name);
					ret = TJS_S_OK;
				} else {
					log(L"QueryControl failed %ws", name);
				}

				// external に global を登録する
				{
					iTJSDispatch2 * global = TVPGetScriptDispatch();
					IDispatchEx *dispatchEx = new IDispatchWrapper(global);
					SetExternalDispatch(dispatchEx);
					dispatchEx->Release();
					global->Release();
				}
				
			} else {
				log(L"CreateControl failed %ws", name);
			}
			SysFreeString(oleName);
		}
		return ret;
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// オブジェクトが無効化されるときに呼ばれる
		NI_WIN32OLE::Invalidate();
		if (m_hWnd) { DestroyWindow(); }
	}

	// -----------------------------------------------------------------------

	/**
	 * 外部拡張ハンドラの登録
	 * ※事実上 IE 専用の処理
	 */
	tjs_error setExternalUI(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		externalUI = new CExternalUI();
		SetExternalUIHandler(externalUI);
		return S_OK;
	}

	// -----------------------------------------------------------------------
	
	void setVisible(bool visible) {
		if (m_hWnd) {
			if (visible) {
				setPos();
			}
			ShowWindow(visible);
		}
	}

	bool getVisible() {
		return m_hWnd && IsWindowVisible();
	}

protected:
	void setPos() {
		if (m_hWnd) {
			SetWindowPos(0, left, top, width, height, 0);
		}
	}

public:
	void setLeft(int l) {
		left = l;
		setPos();
	}

	int getLeft() {
		return left;
	}

	void setTop(int t) {
		top = t;
		setPos();
	}

	int getTop() {
		return top;
	}
	
	void setWidth(int w) {
		width = w;
		setPos();
	}

	int getWidth() {
		return width;
	}

	void setHeight(int h) {
		height = h;
		setPos();
	}

	int getHeight() {
		return height;
	}
	
	/**
	 * 窓場所指定
	 */	
	void setPos(int l, int t) {
		left = l;
		top  = t;
		setPos();
	}

	/**
	 * 窓サイズ指定
	 */	
	void setSize(int w, int h) {
		width = w;
		height = h;
		setPos();
	}

	BEGIN_MSG_MAP(NI_ActiveX)
	END_MSG_MAP()
};

//---------------------------------------------------------------------------
/*
	これは NI_ActiveX のオブジェクトを作成して返すだけの関数です。
	後述の TJSCreateNativeClassForPlugin の引数として渡します。
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_ActiveX()
{
	return new NI_ActiveX();
}
//---------------------------------------------------------------------------
/*
	TJS2 のネイティブクラスは一意な ID で区別されている必要があります。
	これは後述の TJS_BEGIN_NATIVE_MEMBERS マクロで自動的に取得されますが、
	その ID を格納する変数名と、その変数をここで宣言します。
	初期値には無効な ID を表す -1 を指定してください。
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_ActiveX
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;

//---------------------------------------------------------------------------
/*
 *	TJS2 用の「クラス」を作成して返す関数です。
*/
static iTJSDispatch2 * Create_NC_ActiveX()
{
	// クラスオブジェクト
	tTJSNativeClassForPlugin * classobj = TJSCreateNativeClassForPlugin(TJS_W("WIN32OLE"), Create_NI_ActiveX);

	// ネイティブメンバー
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/WIN32OLE)

		// ---------- ここからこぴぺにつき注意 ------------------
		
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
			return _this->missing(result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/missing)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getConstant) // getConstant メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->getConstant(numparams, param, objthis);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/getConstant)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/addEvent) // addEvent メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->addEvent(numparams, param, objthis);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/addEvent)
			
		// ---------- ここまでこぴぺにつき注意 ------------------

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setExternalUI)
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_ActiveX);
			_this->setExternalUI(numparams, param, objthis);
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setExternalUI)

		// ---------------------------------------------------------------
			
		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setPos)
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_ActiveX);
			if (numparams < 2) return TJS_E_BADPARAMCOUNT;
			_this->setPos(*param[0], *param[1]);
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setPos)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setSize)
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_ActiveX);
			if (numparams < 2) return TJS_E_BADPARAMCOUNT;
			_this->setSize(*param[0], *param[1]);
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setSize)
		
		TJS_BEGIN_NATIVE_PROP_DECL(visible) // visible プロパティ
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				*result = _this->getVisible();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				_this->setVisible((bool)*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(visible)

		TJS_BEGIN_NATIVE_PROP_DECL(left)
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				*result = (tjs_int32)_this->getLeft();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				_this->setLeft(*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(left)

		TJS_BEGIN_NATIVE_PROP_DECL(top)
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				*result = (tjs_int32)_this->getTop();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				_this->setTop(*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(top)

		TJS_BEGIN_NATIVE_PROP_DECL(width)
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				*result = (tjs_int32)_this->getWidth();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				_this->setWidth(*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(width)

		TJS_BEGIN_NATIVE_PROP_DECL(height)
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				*result = (tjs_int32)_this->getHeight();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_ActiveX);
				_this->setHeight(*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(height)
			
	TJS_END_NATIVE_MEMBERS

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

// 吉里吉里のアーカイブにアクセスするための処理
void registArchive();
void unregistArchive();

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

	{
		//-----------------------------------------------------------------------
		// 1 まずクラスオブジェクトを作成
		iTJSDispatch2 * tjsclass = Create_NC_ActiveX();
		
		// 2 tjsclass を tTJSVariant 型に変換
		val = tTJSVariant(tjsclass);
		
		// 3 すでに val が tjsclass を保持しているので、tjsclass は
		//   Release する
		tjsclass->Release();
		
		
		// 4 global の PropSet メソッドを用い、オブジェクトを登録する
		global->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("ActiveX"), // メンバ名 ( かならず TJS_W( ) で囲む )
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
			log(L"OLE 初期化失敗");
		}
	}

	// アーカイブ処理
	registArchive();
	
	// ATL関連初期化
	_Module.Init(NULL, NULL);
	AtlAxWinInit();
	
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

		global->DeleteMember(
			0, // フラグ ( 0 でよい )
			TJS_W("ActiveX"), // メンバ名
			NULL, // ヒント
			global // コンテキスト
			);
	}
	
	// - global を Release する
	if(global) global->Release();

	// ATL 終了
	_Module.Term();

	// アーカイブ終了
	unregistArchive();
	
	if (gOLEInitialized) {
		OleUninitialize();
		gOLEInitialized = false;
	}

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();
	
	return S_OK;
}
//---------------------------------------------------------------------------

