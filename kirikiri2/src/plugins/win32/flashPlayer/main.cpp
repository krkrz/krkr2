#include <windows.h>
#include <stdio.h>
#include "ncbind/ncbind.hpp"

// ログ出力用
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

#import "c:\\windows\\system32\\macromed\\flash\\flash8g.ocx" named_guids

class FlashPlayer : public IOleClientSite,
					public IOleInPlaceSiteWindowless,
					public IOleInPlaceFrame,
					public IStorage,
					public ShockwaveFlashObjects::_IShockwaveFlashEvents
{

public:
	/**
	 * コンストラクタ
	 */
	FlashPlayer(iTJSDispatch2 *objthis)
		 : objthis(objthis), refCount(0), control(NULL), oleobj(NULL),
		   viewObjectEx(NULL), viewObject(NULL), windowless(NULL), conContainer(NULL),
		   conPoint(NULL), conPointId(0), updateFlag(false), hdc(NULL), bmp(NULL), pixels(NULL)
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~FlashPlayer() {
		if (conPoint) {
			if (conPointId) {
				conPoint->Unadvise(conPointId);
			}
			conPoint->Release();
		}
		if (conContainer) {
			conContainer->Release();
		}
		if (viewObjectEx) {
			viewObjectEx->Release();
		}
		if (viewObject){
			viewObject->Release();
		}
		if (windowless) {
			windowless->Release();
		}
		if (control) {
			control->Release();
		}
		if (oleobj) {
			oleobj->Close(OLECLOSE_NOSAVE);
			oleobj->Release();
		}
		if (hdc) {
			::DeleteDC(hdc);
		}
		if (bmp) {
			::DeleteObject(bmp);
		}
	}

	/**
	 * インスタンス生成ファクトリ
	 */
	static tjs_error factory(FlashPlayer **result, tjs_int numparams, tTJSVariant **params, iTJSDispatch2 *objthis) {
		if (numparams < 2) {
			return TJS_E_BADPARAMCOUNT;
		}
		FlashPlayer *self = new FlashPlayer(objthis);
		if (self->Create(*params[0], *params[1])) {
			*result = self;
			return TJS_S_OK;
		} else {
			delete self;
			return TJS_E_FAIL;
		}
	}
	
public:

#define CHECK if(!control){TVPThrowExceptionMessage(L"not initialized");}
	
	long getReadyState() {
		CHECK;
		return control->GetReadyState();
	}

	long getTotalFrames() {
		CHECK;
		return control->GetTotalFrames();
	}
	
	bool getPlaying (){
		CHECK;
		return control->GetPlaying() != VARIANT_FALSE;
	}

	void setPlaying (bool val) {
		CHECK;
		control->PutPlaying(val ? VARIANT_TRUE : VARIANT_FALSE);
	}

	int getQuality () {
		CHECK;
		return control->GetQuality();
	}
	void setQuality (int val) {
		CHECK;
		control->PutQuality(val);
	}

	int getScaleMode() {
		CHECK;
		return control->GetScaleMode();
	}
	void setScaleMode(int val) {
		CHECK;
		control->PutScaleMode(val);
	}

	int getAlignMode() {
		CHECK;
		return control->GetAlignMode();
	}
	void setAlignMode(int val) {
		CHECK;
		control->PutAlignMode(val);
	}

	long getBackgroundColor() {
		CHECK;
		return control->GetBackgroundColor();
	}
	void setBackgroundColor (int val) {
		CHECK;
		control->PutBackgroundColor(val);
	}

	bool getLoop() {
		CHECK;
		return control->GetLoop() != VARIANT_FALSE;
	}
	void setLoop(bool val) {
		CHECK;
		control->PutLoop (val ? VARIANT_TRUE : VARIANT_FALSE);
	}

	ttstr getMovie() {
		CHECK;
		return ttstr((const wchar_t*)control->GetMovie());
	}
	void setMovie(const tjs_char *val) {
		CHECK;
		control->PutMovie(val);
	}

	int getFrameNum() {
		CHECK;
		return control->GetFrameNum();
	}
	void setFrameNum (int val) {
		CHECK;
		control->PutFrameNum(val);
	}

	bool setZoomRect(int left, int top, int right, int bottom) {
		CHECK;
		return SUCCEEDED(control->SetZoomRect(left, top, right, bottom));
	}

	bool zoom(int factor) {
		CHECK;
		return SUCCEEDED(control->Zoom(factor));
	}
	bool pan(int x, int y, int mode ) {
		CHECK;
		return SUCCEEDED(control->Pan(x,y,mode));
	}

	bool play() {
		CHECK;
		return SUCCEEDED(control->Play());
	}

	bool stop() {
		CHECK;
		return SUCCEEDED(control->Stop());
	}

	bool back() {
		CHECK;
		return SUCCEEDED(control->Back());
	}

	bool forward() {
		CHECK;
		return SUCCEEDED(control->Forward());
	}
	bool rewind() {
		CHECK;
		return SUCCEEDED(control->Rewind());
	}
	bool stopPlay() {
		CHECK;
		return SUCCEEDED(control->StopPlay());
	}
	bool gotoFrame(int frameNum) {
		CHECK;
		return SUCCEEDED(control->GotoFrame(frameNum));
	}

#if 0
	long CurrentFrame();
    VARIANT_BOOL IsPlaying();
    long PercentLoaded();
    VARIANT_BOOL FrameLoaded (
        long FrameNum );
    long FlashVersion();
    _bstr_t GetWMode();
    void PutWMode (
        _bstr_t pVal );
    _bstr_t GetSAlign();
    void PutSAlign (
        _bstr_t pVal );
    VARIANT_BOOL GetMenu();
    void PutMenu (
        VARIANT_BOOL pVal );
    _bstr_t GetBase();
    void PutBase (
        _bstr_t pVal );
    _bstr_t GetScale();
    void PutScale (
        _bstr_t pVal );
    VARIANT_BOOL GetDeviceFont();
    void PutDeviceFont (
        VARIANT_BOOL pVal );
    VARIANT_BOOL GetEmbedMovie();
    void PutEmbedMovie (
        VARIANT_BOOL pVal );
    _bstr_t GetBGColor();
    void PutBGColor (
        _bstr_t pVal );
    _bstr_t GetQuality2();
    void PutQuality2 (
        _bstr_t pVal );
#endif
	
	bool loadMovie(int layer, const tjs_char *url) {
		return SUCCEEDED(control->LoadMovie(layer, url));
	}

#if 0
    HRESULT TGotoFrame (
        _bstr_t target,
        long FrameNum );
    HRESULT TGotoLabel (
        _bstr_t target,
        _bstr_t label );
    long TCurrentFrame (
        _bstr_t target );
    _bstr_t TCurrentLabel (
        _bstr_t target );
    HRESULT TPlay (
        _bstr_t target );
    HRESULT TStopPlay (
        _bstr_t target );
    HRESULT SetVariable (
        _bstr_t name,
        _bstr_t value );
    _bstr_t GetVariable (
        _bstr_t name );
    HRESULT TSetProperty (
        _bstr_t target,
        int property,
        _bstr_t value );
    _bstr_t TGetProperty (
        _bstr_t target,
        int property );
    HRESULT TCallFrame (
        _bstr_t target,
        int FrameNum );
    HRESULT TCallLabel (
        _bstr_t target,
        _bstr_t label );
    HRESULT TSetPropertyNum (
        _bstr_t target,
        int property,
        double value );
    double TGetPropertyNum (
        _bstr_t target,
        int property );
    double TGetPropertyAsNumber (
        _bstr_t target,
        int property );
    _bstr_t GetSWRemote ( );
    void PutSWRemote (
        _bstr_t pVal );
    _bstr_t GetFlashVars ( );
    void PutFlashVars (
        _bstr_t pVal );
    _bstr_t GetAllowScriptAccess ( );
    void PutAllowScriptAccess (
        _bstr_t pVal );
    _bstr_t GetMovieData ( );
    void PutMovieData (
        _bstr_t pVal );
    IUnknownPtr GetInlineData ( );
    void PutInlineData (
        IUnknown * ppIUnknown );
    VARIANT_BOOL GetSeamlessTabbing ( );
    void PutSeamlessTabbing (
        VARIANT_BOOL pVal );
    HRESULT EnforceLocalSecurity ( );
    VARIANT_BOOL GetProfile ( );
    void PutProfile (
        VARIANT_BOOL pVal );
    _bstr_t GetProfileAddress ( );
    void PutProfileAddress (
        _bstr_t pVal );
    long GetProfilePort ( );
    void PutProfilePort (
        long pVal );
    _bstr_t CallFunction (
        _bstr_t request );
    HRESULT SetReturnValue (
        _bstr_t returnValue );
    HRESULT DisableLocalSecurity ( );
#endif
	
	/**
	 * サイズを指定
	 * @param width 横幅
	 * @param height 縦幅
	 */
	virtual void setSize(int width, int height) {
		RECT rect = {0,0,width,height};
		if (windowless) {
			windowless->SetObjectRects(&rect, &rect);
		}
		if (!hdc || !EqualRect(&rect, &size)) {
			if (hdc) {
				::DeleteDC(hdc);
			}
			if (bmp) {
				::DeleteObject(bmp);
			}
			this->width  = width;
			this->height = height;
			this->pitch  = (width * 4 + 3) / 4 * 4;
			updateRect = size = rect;
			updateFlag = true;
			BITMAPINFOHEADER bih = {0};
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biPlanes = 1;
			bih.biWidth  = width;
			bih.biHeight = -height;
			hdc = CreateCompatibleDC(NULL);
			bmp = CreateDIBSection(hdc, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&pixels, NULL, 0x0);
			SelectObject(hdc, bmp);
		}
	}

	/**
	 * 入力判定
	 * @param x
	 * @param y
	 */
	bool hitTest(int x, int y) {
		if (viewObjectEx) {
			POINT p = {x, y};
			DWORD dwRes;
			HRESULT hr = viewObjectEx->QueryHitPoint(DVASPECT_CONTENT, &size, p, 1, &dwRes);
			if (hr == S_OK)	{
				return dwRes != HITRESULT_OUTSIDE;
			}
		}
		return false;
	}

	/**
	 * レイヤに対して内容を全描画
	 * @param layer レイヤ
	 * @param onlyUpdate 更新部のみ描画
	 */
	void _draw(iTJSDispatch2 *layer, bool onlyUpdate=false) {

		if (!hdc) {
			return;
		}

		if (updateFlag) {
			IViewObject *lpV = viewObjectEx ? (IViewObject *)viewObjectEx : viewObject;
			HRGN rgn = CreateRectRgn(updateRect.left, updateRect.top, updateRect.right+1, updateRect.bottom+1);
			SelectClipRgn(hdc, rgn);
			FillRect(hdc, &size, (HBRUSH)GetStockObject(BLACK_BRUSH));
			OleDraw(lpV, DVASPECT_TRANSPARENT, hdc, &size);
			DeleteObject(rgn);
		}
		
		ncbPropAccessor obj(layer);
		int iw = obj.getIntValue(L"imageWidth");
		int ih = obj.getIntValue(L"imageHeight");
		unsigned char *imageBuffer = (unsigned char*)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
		tjs_int imagePitch = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());

		//log(L"draw:%d %d", onlyUpdate, updateFlag);
		if (onlyUpdate) {
			if (updateFlag) {
				int l = updateRect.left;
				int t = updateRect.top;
				int w = updateRect.right - l;
				int h = updateRect.bottom - t;
				
				//log(L"before l:%d t:%d w:%d h:%d iw:%d ih:%d", l, t, w, h, iw, ih);
				if (l+w > iw) {
					w -= (l+w - iw);
				}
				if (t+h > ih) {
					h -= (t+h - ih);
				}
				//log(L"after l:%d t:%d w:%d h:%d", l, t, w, h);
				if (w >0 && h >0) {
					int line = w * 4;
					const tjs_uint8 *src = (tjs_uint8*)(pixels) + pitch * t + l*4;
					tjs_uint8 *dst = imageBuffer + imagePitch * t + l * 4;
					for (tjs_int y = 0; y < h; y++) {
						CopyMemory(dst, src, line);
						src += pitch;
						dst += imagePitch;
					}
					tTJSVariant  vars [4] = { l, t, w, h };
					tTJSVariant *varsp[4] = { vars, vars+1, vars+2, vars+3 };
					layer->FuncCall(0, L"update", NULL, NULL, 4, varsp, layer);
				}
			}
		} else {
			int w = width;
			int h = height;
			if (iw < w) {
				w = iw;
			}
			if (ih < h) {
				h = ih;
			}
			int line = w * 4;
			const tjs_uint8 *src = (tjs_uint8*)(pixels);
			tjs_uint8 *dst = imageBuffer;
			for (tjs_int y = 0; y < h; y++) {
				CopyMemory(dst, src, line);
				src += pitch;
				dst += imagePitch;
			}
			tTJSVariant  vars [4] = { 0, 0, w, h };
			tTJSVariant *varsp[4] = { vars, vars+1, vars+2, vars+3 };
			layer->FuncCall(0, L"update", NULL, NULL, 4, varsp, layer);
		}
		// 更新情報の抹消
		updateFlag = false;
		SetRect(&updateRect,0,0,0,0);
	}

	static tjs_error draw(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, FlashPlayer *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (params[0]->Type() != tvtObject || params[0]->AsObjectNoAddRef()->IsInstanceOf(0,NULL,NULL,L"Layer",NULL) == false) {
			return TJS_E_INVALIDTYPE;
		}
		self->_draw(*params[0], numparams > 1 && (int)*params[1] != 0);
		return TJS_S_OK;
	}
	
	// XXX マウス入力他差し替え処理を作る
	LRESULT sendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDBLCLK
			|| uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDBLCLK
			|| uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MBUTTONDBLCLK
			|| uMsg == WM_MOUSEWHEEL 
			|| uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR
			|| uMsg == WM_SETCURSOR
			)
		{
			LRESULT res;
			if (SUCCEEDED(windowless->OnWindowMessage(uMsg, wParam, lParam, &res))) {
				return res;
			}
		}
		return 0;
	}

	
public:
	//interface methods
	
	//IUnknown 
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject) {
		if (IsEqualGUID(riid, IID_IUnknown))
			*ppvObject = (void*)(this);
		else if (IsEqualGUID(riid, IID_IOleInPlaceSite))
			*ppvObject = (void*)dynamic_cast<IOleInPlaceSite *>(this);
		else if (IsEqualGUID(riid, IID_IOleInPlaceSiteEx))
			*ppvObject = (void*)dynamic_cast<IOleInPlaceSiteEx *>(this);
		else if (IsEqualGUID(riid, IID_IOleInPlaceSiteWindowless))
			*ppvObject = (void*)dynamic_cast<IOleInPlaceSiteWindowless *>(this);
		else if (IsEqualGUID(riid, IID_IOleInPlaceFrame))
			*ppvObject = (void*)dynamic_cast<IOleInPlaceFrame *>(this);
		else if (IsEqualGUID(riid, IID_IStorage))
			*ppvObject = (void*)dynamic_cast<IStorage *>(this);
		else if (IsEqualGUID(riid, ShockwaveFlashObjects::DIID__IShockwaveFlashEvents))
			*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::_IShockwaveFlashEvents *>(this);
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
		if (!(*ppvObject))
			return E_NOINTERFACE; //if dynamic_cast returned 0
		refCount++;
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		refCount++;
		return refCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		refCount--;
		return refCount;
	}

	//IOleClientSite
	virtual HRESULT STDMETHODCALLTYPE SaveObject() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER FAR* ppContainer) { *ppContainer = 0; return E_NOINTERFACE; }
	virtual HRESULT STDMETHODCALLTYPE ShowObject() { return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() { return E_NOTIMPL; }

	//IOleInPlaceSite
	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd) {	return E_FAIL; }
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate() { return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate() { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate() { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo() { return E_NOTIMPL;} 
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect) { return S_OK; }

	//IOleInPlaceSiteEx
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags) {
		if (pfNoRedraw)
			*pfNoRedraw = FALSE;
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL fNoRedraw) { return S_FALSE; }
	virtual HRESULT STDMETHODCALLTYPE RequestUIActivate(void) { return S_FALSE; }

	//IOleInPlaceSiteWindowless
	virtual HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetCapture(void) { return S_FALSE; }
	virtual HRESULT STDMETHODCALLTYPE SetCapture(/* [in] */ BOOL fCapture) { return S_FALSE; }
	virtual HRESULT STDMETHODCALLTYPE GetFocus(void) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE SetFocus(/* [in] */ BOOL fFocus) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetDC(/* [in] */ LPCRECT pRect, /* [in] */ DWORD grfFlags, /* [out] */ HDC __RPC_FAR *phDC) {	return S_FALSE; }
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(/* [in] */ HDC hDC) { return S_FALSE; }
	virtual HRESULT STDMETHODCALLTYPE InvalidateRect(/* [in] */ LPCRECT pRect, /* [in] */ BOOL fErase) {
		if (pRect) {
			RECT u;
			updateFlag = UnionRect(&u, &updateRect, pRect) != 0;
			updateRect = u;
			static ttstr eventName(TJS_W("onFrameUpdate"));
			TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_IDLE | TVP_EPT_REMOVE_POST, 0, NULL);
		}
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE InvalidateRgn(/* [in] */ HRGN hRGN, /* [in] */ BOOL fErase) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE ScrollRect( 
		/* [in] */ INT dx,
		/* [in] */ INT dy,
		/* [in] */ LPCRECT pRectScroll,
		/* [in] */ LPCRECT pRectClip) {
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE AdjustRect( /* [out][in] */ LPRECT prc) { return S_FALSE; }
	virtual HRESULT STDMETHODCALLTYPE OnDefWindowMessage( 
		/* [in] */ UINT msg,
		/* [in] */ WPARAM wParam,
        /* [in] */ LPARAM lParam,
		/* [out] */ LRESULT __RPC_FAR *plResult) { return S_FALSE; }

	//IOleInPlaceFrame
//	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
//	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	virtual HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID) { return E_NOTIMPL; }

	//IStorage
	virtual HRESULT STDMETHODCALLTYPE CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Revert() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR *pwcsName) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask)  { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG * pstatstg, DWORD grfStatFlag)  { return E_NOTIMPL; }

	//IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
        /* [out] */ UINT __RPC_FAR *pctinfo) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
		/* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID __RPC_FAR *rgDispId) { return E_NOTIMPL; }
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
        /* [out] */ VARIANT __RPC_FAR *pVarResult,
        /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
		/* [out] */ UINT __RPC_FAR *puArgErr) { return S_OK; }

public:
	//DShockwaveFlashEvents
	HRESULT STDMETHODCALLTYPE OnReadyStateChange (long newState) {
		tTJSVariant param(newState);
		static ttstr eventName(TJS_W("onReadyStateChange"));
		TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_POST, 1, &param);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnProgress (long percentDone) {
		tTJSVariant param(percentDone);
		static ttstr eventName(TJS_W("onProgress"));
		TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_POST, 1, &param);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FSCommand (_bstr_t command, _bstr_t args) {
		tTJSVariant params[2];
		params[0] = (wchar_t*)command;
		params[1] = (wchar_t*)args;
		static ttstr eventName(TJS_W("onFSCommand"));
		TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_POST, 2, params);
		return S_OK;
	}

protected:

	/**
	 * コントロール生成
	 * @param width  コントロール横幅
	 * @param height コントロール縦幅
	 * @return コントロールの生成に成功したかどうか
	 */
	BOOL Create(int width, int height) {
		if (FAILED(OleCreate(ShockwaveFlashObjects::CLSID_ShockwaveFlash, IID_IOleObject, OLERENDER_DRAW,
							 0, (IOleClientSite *)this, (IStorage *)this, (void **)&oleobj))) {
			return FALSE;
		}
		if (FAILED(OleSetContainedObject(oleobj, TRUE))) {
			return FALSE;
		}
		if (FAILED(oleobj->QueryInterface(IID_IViewObjectEx, (void **)&viewObjectEx))) {
			viewObjectEx = NULL;
			if (FAILED(oleobj->QueryInterface(IID_IViewObject, (void **)&viewObject))) {
				return FALSE;
			}
		}
		if (FAILED(oleobj->QueryInterface(IID_IOleInPlaceObjectWindowless, (void **)&windowless))) {
			return FALSE;
		}
		if (FAILED(oleobj->QueryInterface(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void **)&control))) {
			return FALSE;
		}
		if (FAILED(control->QueryInterface(IID_IConnectionPointContainer, (void**)&conContainer))) {
			return FALSE;
		}
		if (FAILED(conContainer->FindConnectionPoint(ShockwaveFlashObjects::DIID__IShockwaveFlashEvents, &conPoint))) {
			return FALSE;
		}
		if (FAILED(conPoint->Advise((ShockwaveFlashObjects::_IShockwaveFlashEvents *)this, &conPointId))) {
			return FALSE;
		}

		control->PutWMode(L"transparent");
		setSize(width, height);
		
		if (FAILED(oleobj->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)this, 0, NULL, NULL))) {
			return FALSE;
		}
		return TRUE;
	}

private:
	iTJSDispatch2 *objthis; ///< 自己オブジェクト情報の参照
	int refCount; ///< リファレンスカウント
	
	IOleObject *oleobj;
	ShockwaveFlashObjects::IShockwaveFlash *control;
	IViewObject *viewObject;
	IViewObjectEx *viewObjectEx;
	IOleInPlaceObjectWindowless *windowless;

	IConnectionPointContainer *conContainer;
	IConnectionPoint *conPoint;
	DWORD conPointId;

	int width;
	int height;
	int pitch;
	RECT size;
	RECT updateRect;
	bool updateFlag;
	HDC hdc;
	HBITMAP bmp;
	BYTE *pixels;
};

NCB_REGISTER_CLASS(FlashPlayer) {
	Factory(&ClassT::factory);
	NCB_METHOD(setSize);
	NCB_METHOD(hitTest);
	RawCallback(TJS_W("draw"), &Class::draw, 0);

	NCB_PROPERTY_RO(readyState, getReadyState);
	NCB_PROPERTY_RO(totalFrames, getTotalFrames);
	NCB_PROPERTY(playing, getPlaying, setPlaying);
	NCB_PROPERTY(quality, getQuality, setQuality);
	NCB_PROPERTY(scaleMode, getScaleMode, setScaleMode);
	NCB_PROPERTY(alighMode, getAlignMode, setAlignMode);
	NCB_PROPERTY(backgroundColor, getBackgroundColor, setBackgroundColor);
	NCB_PROPERTY(loop, getLoop, setLoop);
	NCB_PROPERTY(movie, getMovie, setMovie);
	NCB_PROPERTY(frameNum, getFrameNum, setFrameNum);

	NCB_METHOD(setZoomRect);
	NCB_METHOD(zoom);
	NCB_METHOD(pan);
	NCB_METHOD(play);
	NCB_METHOD(stop);
	NCB_METHOD(back);
	NCB_METHOD(forward);
	NCB_METHOD(rewind);
	NCB_METHOD(stopPlay);
	NCB_METHOD(gotoFrame);
	NCB_METHOD(loadMovie);
};

//---------------------------------------------------------------------------

// 吉里吉里のアーカイブにアクセスするための処理
void registArchive();
void unregistArchive();

static BOOL gOLEInitialized = false;

/**
 * 登録処理前
 */
static void PreRegistCallback()
{
	if (!gOLEInitialized) {
		if (SUCCEEDED(OleInitialize(NULL))) {
			gOLEInitialized = true;
		} else {
			log(L"OLE 初期化失敗");
		}
	}
	
	// アーカイブ処理
//	registArchive();
}

/**
 * 登録処理後
 */
static void PostRegistCallback()
{
}

/**
 * 開放処理前
 */
static void PreUnregistCallback()
{
}

/**
 * 開放処理後
 */
static void PostUnregistCallback()
{
	// アーカイブ終了
//	unregistArchive();
	
	if (gOLEInitialized) {
		OleUninitialize();
		gOLEInitialized = false;
	}
}


NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
