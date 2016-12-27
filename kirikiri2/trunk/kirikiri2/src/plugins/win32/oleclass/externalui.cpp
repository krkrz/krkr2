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

IDocHostUIHandlerDispatch *createExternalUI()
{
	return new CExternalUI();
}
