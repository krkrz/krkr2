#include <windows.h>
#include <DispEx.h>
#include "tp_stub.h"

//---------------------------------------------------------------------------

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

/**
 * シンク（イベントディスパッチャ）クラス
 * リンクのSHIFT＋クリックでの別ウィンドウオープンを回避する為に、
 * DISPID_NEWWINDOWをフックする。
 */ 

class ATL_NO_VTABLE VNBSink :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatch
{
public:
	CComPtr<IWebBrowser2> m_pBrowser;
	CWindow *showWhenComplete;
	bool disableControl;
	
	VNBSink() {
		m_pBrowser = NULL;
		showWhenComplete = NULL;
		disableControl = true;
	}

	BEGIN_COM_MAP(VNBSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, IDispatch)
	END_COM_MAP()

// IDispatch
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
		HRESULT hr = S_OK;
		if ( pdispparams ) {
			switch ( dispidMember ) {
			case DISPID_DOCUMENTCOMPLETE:
				if (m_pBrowser) {
					CComPtr<IDispatch> pDisp;
					m_pBrowser->get_Document(&pDisp);
					if (pDisp) {
						CComQIPtr<IHTMLDocument2> pDoc = pDisp;
						if (pDoc) {
							USES_CONVERSION;
							CComBSTR str;
							pDoc->get_title(&str);
							showWhenComplete->SetWindowText(OLE2T(str));
						}
					}
				}
				if (showWhenComplete) {
					showWhenComplete->ShowWindow(TRUE);
				}
				break;
			case DISPID_NEWWINDOW:
			case DISPID_NEWWINDOW2:
			case DISPID_FRAMENEWWINDOW:
				// 現在のウィンドウでそのままオープンする
				//if ( m_pBrowser ) {
				//	VARIANT defArg;
				//	defArg.vt = VT_ERROR;
				//	defArg.scode = DISP_E_PARAMNOTFOUND;
				//	m_pBrowser->Navigate(pdispparams->rgvarg[5].bstrVal, &defArg, &defArg, pdispparams->rgvarg[2].pvarVal, &defArg);
				//}
				if (disableControl) {
					*(pdispparams->rgvarg[0].pboolVal) = VARIANT_TRUE;
				}
				break;
			default:
				break;
			}
		} else {
			hr = DISP_E_PARAMNOTFOUND;
		}
		return hr;
	}
};


/**
 * DHTMLの外部呼出し(window.external)と各種UI処理を受理するためのクラス
 * CAxWindow::SetExternalUIHandler で設定することを想定している。
 * getExternal メソッドで、外部呼び出し用の IDispatch を返している。
 *
 * 現在実装されているメソッド
 * window.external.SetBackColor 背景色を設定するメソッド
 *
 * それぞれクラス中の対応するメソッドを呼び出している。継承して中身を記述することで
 * 挙動が変更できる。パラメータはCOMスタイルのもの(VARIANT)になるので注意
 */ 
class CHtmlExt : public IDocHostUIHandlerDispatch {

protected:
	IDispatchEx *dispatchEx;

public:
	CHtmlExt() {
		dispatchEx = NULL;
	}

	~CHtmlExt() {
		if (dispatchEx) {
			dispatchEx->Release();
		}
	}
	
	//----------------------------------------------------------------------------
	// IUnknown 実装
	
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
											 void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (dispatchEx && (riid == IID_IUnknown || riid == IID_IDispatch || riid == IID_IDispatchEx)) {
			//TVPAddLog(TJS_W("get dispatchEx"));
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

	ULONG STDMETHODCALLTYPE AddRef() {
		return 1;
	}

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

class CBrowserWindow : public CWindowImpl<CBrowserWindow, CAxWindow>, public CHtmlExt
{
protected:
	bool disableControl; // 制御制限用フラグ

public:

	LRESULT OnKeydown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){

		// browser is a data member of type IWebBrowser2.
		// Using CComQIPtr in this way queries browser
		// for the IOleInPlaceActiveObject interface which is
		// then stored in the pIOIPAO variable.
		//
		CComQIPtr<IOleInPlaceActiveObject,
		&IID_IOleInPlaceActiveObject> pIOIPAO(browser);
		
		HRESULT hr = S_FALSE;

		// 一部の機能を殺す為にアクセラレータを呼ばないようにする
		switch ( wParam ) {
		case 'A': // 全選択
		case 'C': // コピー
		case 'X': // カット
		case 'V': // ペースト
		case 'Z': // UNDO
		case VK_TAB:    // タブ(次の項目)
		case VK_INSERT: // 各種こぴぺ処理?
		case VK_DELETE: // 選択削除
			if (pIOIPAO) {
				MSG msg;
				msg.message = uMsg;
				msg.wParam = wParam;
				msg.lParam = lParam;
				hr = pIOIPAO->TranslateAccelerator(&msg);
			}
			break;
		//case 'N':		// 新規ウィンドウ
		//case 'P':		// 印刷
		//case 'O':		// オープン
		//case 'F':		// 検索
		default:
			break;
		}
		return hr;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
		bHandled = true;
		setVisible(false);
		return S_OK;
	}
	
private:
	// ブラウザのコントローラ
	DWORD dwCookie;
	CComPtr<IUnknown> pSink;
public:
	CComPtr<IWebBrowser2> browser;

public:

	/**
	 * 生成時処理
	 */
	CBrowserWindow(HWND hwnd, bool disableControl = true) : browser(0) {

		HRESULT hr;
		this->disableControl = disableControl;
		
		// 親サイズを取得
		RECT rect;
		if (hwnd) {
			::GetClientRect(hwnd, &rect);
		}
		
		// HTML コントロール作成
		if (Create(hwnd, hwnd ? rect : rcDefault, NULL, hwnd ? WS_CHILD|WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN)) {
			hr = CreateControl(OLESTR("Shell.Explorer.2"));
			if (SUCCEEDED(hr)) {
				// 自分自身を external 処理用に登録する。
				// SetExternalDispatch が本来のこの目的のメソッドだが
				// IDocHostUIHandlerDispatch を使う場合はそれの GetExternal が優先されて
				// 無視されるので無意味になる。SetExternalUIHandler での一括指定を行う。
				if (disableControl) {
					hr = SetExternalUIHandler(static_cast<CHtmlExt*>(this));
				} else {
					hr = SetExternalDispatch(static_cast<CHtmlExt*>(this));
				}
				if (SUCCEEDED(hr)) {
					
					// インターフェース取得
					hr = QueryControl(&browser);
					
					// シンクオブジェクトの作成
					CComObject<VNBSink>* sink;
					CComObject<VNBSink>::CreateInstance(&sink);
					
					// イベントDispatchの指示
					hr = sink->QueryInterface(IID_IUnknown, (void**)&pSink);
					if ( SUCCEEDED(hr) ) {
						hr = AtlAdvise(browser, pSink, DIID_DWebBrowserEvents2, &dwCookie);
						if ( SUCCEEDED(hr) ) {
							sink->m_pBrowser = browser;
							sink->showWhenComplete = this;
							sink->disableControl = disableControl;
						}
					}
				}
			}
			// XXX ここでIE に対するフック処理が本来いるhook = gemEnvDepWinAddDispatchHook(BrowserDispatchHook, this);
		}
	}

	/**
	 * 削除時処理
	 */
	virtual ~CBrowserWindow() {
		if (browser) {
			if ( pSink ) {
				AtlUnadvise(browser, DIID_DWebBrowserEvents2, dwCookie);
			}
			browser.Release();
			browser.Detach();
		}
		if (m_hWnd) DestroyWindow();
	}
	
	bool isOk() {
		return m_hWnd && browser;
	}
	
	/**
	 * ブラウザ起動
	 * @param path 起動させるURL
	 */
	void open(const tjs_char *path) {

		{
			ttstr msg = "open:";
			msg += path;
			//TVPAddLog(msg);
		}
		
		if (browser) {
			CComVariant vUrl(path);
			CComVariant vempty;
			browser->Navigate2(&vUrl, &vempty, &vempty, &vempty, &vempty);
		}
	}

	virtual void reload() {
		if (browser) browser->Refresh();
	}
	
	void back() {
		if (browser) browser->GoBack();
	}
	
	void forward() {
		if (browser) browser->GoForward();
	}

	void setVisible(bool visible) {
		if (m_hWnd) {
			ShowWindow(visible);
		}
	}
	
	bool getVisible() {
		return m_hWnd && IsWindowVisible();
	}
	
	/**
	 * 窓場所指定
	 */	
	void setPos(int x, int y, int w, int h) {
		RECT rect = {x, y, x+w, y+h};
		SetWindowPos(0, &rect, 0);
	}

	BEGIN_MSG_MAP(CBrowserWindow)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeydown)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()
};

/**
 * メッセージのフック処理。こういったものが登録できないとうまく動かない…
 * TranslateMessage 後、DispatchMessage 前に呼び出される
 * IE に送られるはずのメッセージを横取りして処理できる場所に送る
 */
extern "C" int BrowserDispatchHook(MSG *msg, void *vwork)
{
	CBrowserWindow *br = (CBrowserWindow*)vwork;
	if (IsChild(br->m_hWnd, msg->hwnd)) {
		if ((msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)) {
			// キー入力はIEの親になっている CBrowserWindow に送る
			SendMessage(br->m_hWnd, msg->message, msg->wParam, msg->lParam);
			// さらに IE にも送っておく
//		} else if (msg->message == WM_COMMAND) {
//			// WM_COMMAND はさらに親に送る
//			SendMessage(br->parentHwnd, msg->message, msg->wParam, msg->lParam);
//			// 他に送る必要がないので捨てる
//			return 1;
		}
	}
	return 0;
}

//---------------------------------------------------------------------------

#include "IDispatchWrapper.hpp"

/**
 * 継承して TJS 用に実装
 * windows.external を global にマッピングしてしまう
 */
class TJSBrowserWindow : public CBrowserWindow {
public:
	/// コンストラクタ
	TJSBrowserWindow(HWND hwnd, bool disableControl = true) : CBrowserWindow(hwnd, disableControl) {
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		dispatchEx = new IDispatchWrapper(global);
		global->Release();
	}
};


//---------------------------------------------------------------------------

/*
	プラグイン側でネイティブ実装されたクラスを提供し、吉里吉里側で使用できるように
	する例です。

	ネイティブクラスは tTJSNativeInstance を継承したクラス上に作成し、そのネイ
	ティブクラスとのインターフェースを tTJSNativeClassForPlugin をベースに作成し
	ます。

	「TJS2 リファレンス」の「組み込みの手引き」の「基本的な使い方」にある例と同じ
	クラスをここでは作成します。ただし、プラグインで実装する都合上、TJS2 リファ
	レンスにある例とは若干実装の仕方が異なることに注意してください。
*/

//---------------------------------------------------------------------------
// テストクラス
//---------------------------------------------------------------------------
/*
	各オブジェクト (iTJSDispatch2 インターフェース) にはネイティブインスタンスと
	呼ばれる、iTJSNativeInstance 型のオブジェクトを登録することができ、これを
	オブジェクトから取り出すことができます。
	まず、ネイティブインスタンスの実装です。ネイティブインスタンスを実装するには
	tTJSNativeInstance からクラスを導出します。tTJSNativeInstance は
	iTJSNativeInstance の基本的な動作を実装しています。
*/
class NI_Browser : public tTJSNativeInstance // ネイティブインスタンス
{

public:
	TJSBrowserWindow *browser;

	NI_Browser()
	{
		// コンストラクタ
		/*
			NI_Browser のコンストラクタです。C++ クラスとしての初期化は 後述の
			Construct よりもここで済ませておき、Construct での初期化は最小限の物
			にすることをおすすめします。この例では、データメンバの Value に初期
			値として 0 を設定しています。
		 */
		browser = NULL;
	}

	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		HWND handle = 0;
		bool disableControl = true;
		if (numparams >= 1) {
			iTJSDispatch2 *win = param[0]->AsObjectNoAddRef();
			if (win) {
				tTJSVariant hwnd;
				if (win->PropGet(0, TJS_W("HWND"), NULL, &hwnd, win) == TJS_S_OK) {
					handle = (HWND)(int)hwnd;
				}
			}
			if (numparams >= 2) {
				disableControl = (int)*param[1] != 0;
			}
		}
		browser = new TJSBrowserWindow(handle, disableControl);
		return S_OK;
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// オブジェクトが無効化されるときに呼ばれる
		if (browser) {
			delete browser;
		}
	}
};
//---------------------------------------------------------------------------
/*
	これは NI_Browser のオブジェクトを作成して返すだけの関数です。
	後述の TJSCreateNativeClassForPlugin の引数として渡します。
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Browser()
{
	return new NI_Browser();
}
//---------------------------------------------------------------------------
/*
	TJS2 のネイティブクラスは一意な ID で区別されている必要があります。
	これは後述の TJS_BEGIN_NATIVE_MEMBERS マクロで自動的に取得されますが、
	その ID を格納する変数名と、その変数をここで宣言します。
	初期値には無効な ID を表す -1 を指定してください。
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_Browser
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 用の「クラス」を作成して返す関数です。
*/
static iTJSDispatch2 * Create_NC_Browser()
{
	/*
		まず、クラスのベースとなるクラスオブジェクトを作成します。
		これには TJSCreateNativeClassForPlugin を用います。
		TJSCreateNativeClassForPlugin の第１引数はクラス名、第２引数は
		ネイティブインスタンスを返す関数を指定します。
		作成したオブジェクトを一時的に格納するローカル変数の名前は
		classobj である必要があります。
	*/
	tTJSNativeClassForPlugin * classobj =
		TJSCreateNativeClassForPlugin(TJS_W("Browser"), Create_NI_Browser);


	/*
		TJS_BEGIN_NATIVE_MEMBERS マクロです。引数には TJS2 内で使用するクラス名
		を指定します。
		このマクロと TJS_END_NATIVE_MEMBERS マクロで挟まれた場所に、クラスの
		メンバとなるべきメソッドやプロパティの記述をします。
	*/
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Browser)

		/*
			空の finalize メソッドを宣言します。finalize に相当する処理は
			tTJSNativeInstance::Invalidate をオーバーライドすることでも実装でき
			ますので、通常は空のメソッドで十分です。
		*/
		TJS_DECL_EMPTY_FINALIZE_METHOD

		/*
			(TJSの) コンストラクタを宣言します。TJS でクラスを書くとき、
			クラス内でクラスと同名のメソッドを宣言している部分に相当します。

			TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL マクロの１番目の引数はネイティブ
			インスタンスに割り当てる変数名で、２場面目の引数はその変数の型名です。
			この例でのこのブロック内では NI_Browser * _this という変数が利用可能で、
			ネイティブインスタンスにアクセスすることができます。
			マクロの３番目の引数は、TJS 内で使用するクラス名を指定します。
			TJS_END_NATIVE_CONSTRUCTOR_DECL マクロの引数も同様です。
			ここも、コンストラクタに相当する処理は tTJSNativeInstance::Construct
			をオーバーライドする事で実装できるので、ここでは何もせずに S_OK を返
			します。
		*/
		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Browser,
			/*TJS class name*/Browser)
		{
			// NI_Browser::Construct にも内容を記述できるので
			// ここでは何もしない
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Browser)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/open) // open メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			if (numparams < 1) return TJS_E_BADPARAMCOUNT;
			_this->browser->open(param[0]->GetString());
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/open)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setPos) // setPos メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			if (numparams < 4) return TJS_E_BADPARAMCOUNT;
			_this->browser->setPos(*param[0], *param[1], *param[2], *param[3]);
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setPos)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/back) // back メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			_this->browser->back();
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/back)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/forward) // forward メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			_this->browser->forward();
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/forward)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/reload) // reload メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			_this->browser->reload();
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/reload)

		TJS_BEGIN_NATIVE_PROP_DECL(visible) // visible プロパティ
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_Browser);
				*result = _this->browser->getVisible();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_Browser);
				_this->browser->setVisible((bool)*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(visible)

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
		iTJSDispatch2 * tjsclass = Create_NC_Browser();
		
		// 2 tjsclass を tTJSVariant 型に変換
		val = tTJSVariant(tjsclass);
		
		// 3 すでに val が tjsclass を保持しているので、tjsclass は
		//   Release する
		tjsclass->Release();
		
		
		// 4 global の PropSet メソッドを用い、オブジェクトを登録する
		global->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("Browser"), // メンバ名 ( かならず TJS_W( ) で囲む )
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

	// TJS のグローバルオブジェクトに登録した Browser クラスなどを削除する

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
			TJS_W("Browser"), // メンバ名
			NULL, // ヒント
			global // コンテキスト
			);
	}
	
	// - global を Release する
	if(global) global->Release();

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	_Module.Term();

	unregistArchive();
	
	return S_OK;
}
//---------------------------------------------------------------------------

