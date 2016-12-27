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

IDispatch *createEventSink(GUID diid, ITypeInfo *pTypeInfo, iTJSDispatch2 *receiver)
{
	return new EventSink(diid, pTypeInfo, receiver);
}
