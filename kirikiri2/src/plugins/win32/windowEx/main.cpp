#include <windows.h>
#include "ncbind.hpp"

// ウィンドウクラス名取得用のバッファサイズ
#define CLASSNAME_MAX 1024

// イベント名一覧
#define EXEV_MINIMIZE  TJS_W("onMinimize")
#define EXEV_MAXIMIZE  TJS_W("onMaximize")
#define EXEV_QUERYMAX  TJS_W("onMaximizeQuery")
#define EXEV_SHOW      TJS_W("onShow")
#define EXEV_HIDE      TJS_W("onHide")
#define EXEV_RESIZING  TJS_W("onResizing")
#define EXEV_MOVING    TJS_W("onMoving")
#define EXEV_MOVE      TJS_W("onMove")
#define EXEV_MVSZBEGIN TJS_W("onMoveSizeBegin")
#define EXEV_MVSZEND   TJS_W("onMoveSizeEnd")
#define EXEV_DISPCHG   TJS_W("onDisplayChanged")
#define EXEV_ENTERMENU TJS_W("onEnterMenuLoop")
#define EXEV_EXITMENU  TJS_W("onExitMenuLoop")
#define EXEV_ACTIVATE  TJS_W("onActivateChanged")

////////////////////////////////////////////////////////////////

struct WindowEx
{
	//--------------------------------------------------------------
	// ユーティリティ

	// ネイティブインスタンスポインタを取得
	static inline WindowEx * GetInstance(iTJSDispatch2 *obj) {
		return ncbInstanceAdaptor<WindowEx>::GetNativeInstance(obj);
	}

	// ウィンドウハンドルを取得
	static HWND GetHWND(iTJSDispatch2 *obj) {
		tTJSVariant val;
		obj->PropGet(0, TJS_W("HWND"), 0, &val, obj);
		return (HWND)(tjs_int)(val);
	}

	// RECTを辞書に保存
	static bool SetRect(ncbDictionaryAccessor &rdic, LPCRECT lprect) {
		bool r;
		if ((r = rdic.IsValid())) {
			rdic.SetValue(TJS_W("x"), lprect->left);
			rdic.SetValue(TJS_W("y"), lprect->top);
			rdic.SetValue(TJS_W("w"), lprect->right  - lprect->left);
			rdic.SetValue(TJS_W("h"), lprect->bottom - lprect->top );
		}
		return r;
	}
	// 辞書からRECTに保存
	static void GetRect(LPRECT rect, ncbPropAccessor &dict) {
		ncbTypedefs::Tag<LONG> LongTypeTag;
		rect->left   = dict.GetValue(TJS_W("x"), LongTypeTag);
		rect->top    = dict.GetValue(TJS_W("y"), LongTypeTag);
		rect->right  = dict.GetValue(TJS_W("w"), LongTypeTag);
		rect->bottom = dict.GetValue(TJS_W("h"), LongTypeTag);
		rect->right  += rect->left;
		rect->bottom += rect->top;
	}

	// SYSCOMMANDを送る
	static tjs_error postSysCommand(iTJSDispatch2 *objthis, WPARAM param) {
		::PostMessage(GetHWND(objthis), WM_SYSCOMMAND, param, 0);
		return TJS_S_OK;
	}

	//--------------------------------------------------------------
	// クラス追加メソッド(RawCallback形式)

	// minimize, maximize, showRestore
	static tjs_error TJS_INTF_METHOD minimize(   tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { return postSysCommand(obj, SC_MINIMIZE); }
	static tjs_error TJS_INTF_METHOD maximize(   tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { return postSysCommand(obj, SC_MAXIMIZE); }
	static tjs_error TJS_INTF_METHOD showRestore(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { return postSysCommand(obj, SC_RESTORE);  }

	// resetWindowIcon
	static tjs_error TJS_INTF_METHOD resetWindowIcon(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { 
		HWND hwnd = GetHWND(obj);
		if (hwnd != NULL) {
			HICON icon = LoadIcon(GetModuleHandle(0), IDI_APPLICATION);
			::PostMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
		}
		return TJS_S_OK;
	}

	// getWindowRect
	static tjs_error TJS_INTF_METHOD getWindowRect(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		RECT rect;
		HWND hwnd = GetHWND(obj);
		if (r) r->Clear();
		if (hwnd != NULL && ::GetWindowRect(hwnd, &rect)) {
			ncbDictionaryAccessor dict;
			if (SetRect(dict, &rect) && r) *r = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
	}

	// getClientRect
	static tjs_error TJS_INTF_METHOD getClientRect(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		RECT rect;
		POINT zero = { 0, 0 };
		HWND hwnd = GetHWND(obj);
		if (r) r->Clear();
		if (hwnd != NULL && ::GetClientRect(hwnd, &rect)) {
			::ClientToScreen(hwnd, &zero);
			rect.left   += zero.x; rect.top    += zero.y;
			rect.right  += zero.x; rect.bottom += zero.y;
			ncbDictionaryAccessor dict;
			if (SetRect(dict, &rect) && r) *r = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
	}

	// getNormalRect
	static tjs_error TJS_INTF_METHOD getNormalRect(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		HWND hwnd = GetHWND(obj);
		WINDOWPLACEMENT place;
		ZeroMemory(&place, sizeof(place));
		place.length = sizeof(place);
		if (r) r->Clear();
		if (hwnd != NULL && ::GetWindowPlacement(hwnd, &place)) {
			ncbDictionaryAccessor dict;
			if (SetRect(dict, &place.rcNormalPosition) && r) *r = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
	}

	// property maximized
	static bool isMaximized(iTJSDispatch2 *obj) {
		HWND hwnd = GetHWND(obj);
		return (hwnd != NULL && ::IsZoomed(hwnd));
	}
	static tjs_error TJS_INTF_METHOD getMaximized(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		if (r) *r = isMaximized(obj);
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD setMaximized(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		bool m = !!p[0]->AsInteger();
		if (m != isMaximized(obj)) postSysCommand(obj, m ? SC_MAXIMIZE : SC_RESTORE);
		return TJS_S_OK;
	}

	// property disableResize
	static tjs_error TJS_INTF_METHOD getDisableResize(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		WindowEx *self = GetInstance(obj);
		if (r) *r = (self != NULL && self->disableResize);
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD setDisableResize(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		WindowEx *self = GetInstance(obj);
		if (self == NULL) return TJS_E_ACCESSDENYED;
		self->disableResize = !!p[0]->AsInteger();
		return TJS_S_OK;
	}

	// setOverlayBitmap
	static tjs_error TJS_INTF_METHOD setOverlayBitmap(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		WindowEx *self = GetInstance(obj);
		return (self != NULL) ? self->_setOverlayBitmap(n, p) : TJS_E_ACCESSDENYED;
	}

	//--------------------------------------------------------------
	// 拡張イベント用

	// メンバが存在するか
	bool hasMember(tjs_char const *name) const {
		tTJSVariant func;
		return TJS_SUCCEEDED(self->PropGet(TJS_MEMBERMUSTEXIST, name, 0, &func, self));
	}

	// TJSメソッド呼び出し
	tjs_error funcCall(tjs_char const *name, tTJSVariant *result, tjs_int numparams = 0, tTJSVariant **params = 0) const {
		return self->FuncCall(0, name, 0, result, numparams, params, self);
	}

	// 引数なしコールバック
	bool callback(tjs_char const *name) const {
		if (!hasMember(name)) return false;
		tTJSVariant rslt;
		funcCall(name, &rslt, 0, 0);
		return !!rslt.AsInteger();
	}

	// 座標渡しコールバック
	bool callback(tjs_char const *name, int x, int y) const {
		tTJSVariant vx(x), vy(y);
		tTJSVariant rslt, *params[] = { &vx, &vy };
		funcCall(name, &rslt, 2, params);
		return !!rslt.AsInteger();
	}

	// 矩形渡しコールバック
	bool callback(tjs_char const *name, RECT *rect, int type = 0) const {
		ncbDictionaryAccessor dict;
		if (SetRect(dict, rect)) {
			if (type != 0) dict.SetValue(TJS_W("type"), type);
			tTJSVariant pdic(dict, dict);
			tTJSVariant rslt, *params[] = { &pdic };
			funcCall(name, &rslt, 1, params);
			if (rslt.AsInteger()) {
				GetRect(rect, dict);
				return true;
			}
		}
		return false;
	}

	// メッセージ処理
	bool onMessage(tTVPWindowMessage *mes) {
		switch (mes->Msg) {
		case WM_SYSCOMMAND:
			if ((mes->WParam & 0xFFF0) == SC_MAXIMIZE) return callback(EXEV_QUERYMAX);
			break;
		case WM_SIZE:
			switch (mes->WParam) {
			case SIZE_MINIMIZED: callback(EXEV_MINIMIZE); break;
			case SIZE_MAXIMIZED: callback(EXEV_MAXIMIZE); break;
			}
			break;
		case WM_SHOWWINDOW:
			switch (mes->LParam) {
			case SW_PARENTOPENING: callback(EXEV_SHOW); break;
			case SW_PARENTCLOSING: callback(EXEV_HIDE); break;
			}
			break;
		case WM_ENTERSIZEMOVE: callback(EXEV_MVSZBEGIN); break;
		case WM_EXITSIZEMOVE:  callback(EXEV_MVSZEND);   break;
		case WM_SIZING: if (hasResizing) callback(EXEV_RESIZING, (RECT*)mes->LParam, mes->WParam); break;
		case WM_MOVING: if (hasMoving)   callback(EXEV_MOVING,   (RECT*)mes->LParam); break;
		case WM_MOVE:   if (hasMove)     callback(EXEV_MOVE, (int)LOWORD(mes->LParam), (int)HIWORD(mes->LParam)); break;
			// サイズ変更カーソルを抑制
		case WM_NCHITTEST:
			if (disableResize) {
				HWND hwnd = GetHWND(self);
				if (hwnd != NULL) {
					LRESULT res = ::DefWindowProc(hwnd, mes->Msg, mes->WParam, mes->LParam);
					switch (res) {
						/**/             case HTLEFT:       case HTRIGHT:
					case HTTOP:       case HTTOPLEFT:    case HTTOPRIGHT:
					case HTBOTTOM: case HTBOTTOMLEFT: case HTBOTTOMRIGHT:
						res = HTBORDER;
						break;
					}
					mes->Result = res;
					return true;
				}
			}
			break;
			// システムメニューサイズ変更抑制
		case WM_INITMENUPOPUP:
			if (disableResize && HIWORD(mes->LParam)) {
				HWND hwnd = GetHWND(self);
				if (hwnd != NULL) mes->Result = ::DefWindowProc(hwnd, mes->Msg, mes->WParam, mes->LParam);
				::EnableMenuItem((HMENU)mes->WParam, SC_SIZE, MF_GRAYED | MF_BYCOMMAND);
				return (hwnd != NULL);
			}
			break;

			// メニュー開始終了
		case WM_ENTERMENULOOP:
			callback(EXEV_ENTERMENU);
			break;
		case WM_EXITMENULOOP:
			callback(EXEV_EXITMENU);
			break;

			// ディスプレイモード変更通知
		case WM_DISPLAYCHANGE:
			callback(EXEV_DISPCHG);
			break;

		case WM_ACTIVATE:
			return callback(EXEV_ACTIVATE, (int)(mes->WParam & 0xFFFF), (int)((mes->WParam >> 16) & 0xFFFF));

		case TVP_WM_DETACH:
			if (ovbmp) ovbmp->hide();
			break;
		}
		return false;
	}

	// メッセージレシーバ
	static bool __stdcall receiver(void *userdata, tTVPWindowMessage *mes) {
		WindowEx *inst = GetInstance((iTJSDispatch2*)userdata);
		return inst ? inst->onMessage(mes) : false;
	}

	// Message Receiver 登録・解除
	void regist(bool en) {
		tTJSVariant mode(en ? wrmRegister : wrmUnregister); 
		tTJSVariant func((tTVInteger)(&receiver));
		tTJSVariant data((tTVInteger)(self));
		tTJSVariant rslt, *params[] = { &mode, &func, &data };
		self->FuncCall(0, TJS_W("registerMessageReceiver"), 0, &rslt, 3, params, self);
	}

	// ネイティブインスタンスの生成・破棄にあわせてレシーバを登録・解除する
	WindowEx(iTJSDispatch2 *obj)
		:   self(obj),
			hasResizing(false),
			hasMoving(false),
			hasMove(false),
			disableResize(false),
			ovbmp(NULL)
		{ regist(true); }

	~WindowEx() {
		if (ovbmp) delete ovbmp;
		regist(false);
	}

	void checkExEvents() {
		hasResizing = hasMember(EXEV_RESIZING);
		hasMoving   = hasMember(EXEV_MOVING);
		hasMove     = hasMember(EXEV_MOVE);
	}

protected:
	tjs_error _setOverlayBitmap(tjs_int n, tTJSVariant **p) {
		if (ovbmp) ovbmp->hide();
		if (n > 0 && p[0]->Type() == tvtObject) {
			if (!ovbmp) {
				ovbmp = new OverlayBitmap();
				if (!ovbmp) return TJS_E_FAIL;
			}
			if (!ovbmp->setBitmap(self, p[0]->AsObjectNoAddRef())) {
				ovbmp->hide();
				return TJS_E_FAIL;
			}
		}
		return TJS_S_OK;
	}
private:
	iTJSDispatch2 *self;
	bool hasResizing, hasMoving, hasMove; //< メソッドが存在するかフラグ
	bool disableResize; // サイズ変更禁止

	struct OverlayBitmap {
		OverlayBitmap() : overlay(0), bitmap(0), bmpdc(0) {}
		~OverlayBitmap() {
			if (overlay) ::DestroyWindow(overlay);
			removeBitmap();
		}
		static BOOL CALLBACK SearchScrollBox(HWND hwnd, LPARAM lp) {
			HWND *result = (HWND*)lp;
			tjs_char name[CLASSNAME_MAX];
			::GetClassNameW(hwnd, name, CLASSNAME_MAX);
			if (ttstr(name) == TJS_W("TScrollBox")) {
				*result = hwnd;
				return FALSE;
			}
			return TRUE;
		}
		static LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
			OverlayBitmap *self = reinterpret_cast<OverlayBitmap*>(::GetWindowLong(hwnd, GWL_USERDATA));
			if (self != NULL && msg == WM_PAINT) {
				self->onPaint(hwnd);
				return 0;
			}
			return DefWindowProc(hwnd, msg, wp, lp);
		}
		void drawBitmap(HDC dc) {
			if (bitmap) ::BitBlt(dc, bmpx, bmpy, bmpw, bmph, bmpdc, 0, 0, SRCCOPY);
		}
		void removeBitmap() {
			if (bmpdc)  ::DeleteDC(bmpdc);
			if (bitmap) ::DeleteObject(bitmap);
			bmpdc  = NULL;
			bitmap = NULL;
		}
		void onPaint(HWND hwnd) {
			PAINTSTRUCT ps;
			drawBitmap(::BeginPaint(hwnd, &ps));
			::EndPaint(hwnd, &ps);
		}
		bool setBitmap(iTJSDispatch2 *win, iTJSDispatch2 *lay) {
			typedef unsigned char PIX;

			if (!initOverlay()) return false;
			HWND base = GetHWND(win);
			HWND hwnd = NULL;
			::EnumChildWindows(base, SearchScrollBox, (LPARAM)&hwnd);
			if (!hwnd) return false;

			ncbPropAccessor obj(lay);
			bmpx = obj.getIntValue(TJS_W("left"));
			bmpy = obj.getIntValue(TJS_W("top"));
			bmpw = obj.getIntValue(TJS_W("imageWidth"));
			bmph = obj.getIntValue(TJS_W("imageHeight"));
			tjs_int ln = obj.getIntValue(TJS_W("mainImageBufferPitch"));
			PIX *pw, *pr = reinterpret_cast<unsigned char *>(obj.getIntValue(TJS_W("mainImageBuffer")));

			BITMAPINFO info;
			ZeroMemory(&info, sizeof(info));
			info.bmiHeader.biSize = sizeof(BITMAPINFO);
			info.bmiHeader.biWidth = bmpw;
			info.bmiHeader.biHeight = bmph;
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 24;

			removeBitmap();
			HDC dc = ::GetDC(overlay);
			bitmap = ::CreateDIBSection(dc, (LPBITMAPINFO)&info, DIB_RGB_COLORS, (LPVOID*)&pw, NULL, 0);
			bmpdc  = ::CreateCompatibleDC(dc);
			::SelectObject(bmpdc, bitmap);

			if (!bitmap || !bmpdc) return false;
			for (int y = bmph-1; y >= 0; y--) {
				PIX *src = pr + (y * ln);
				PIX *dst = pw + ((bmph-1 - y) * ((bmpw*3+3) & ~3L));
				for (int n = bmpw-1; n >= 0; n--, src+=4, dst+=3) {
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];
				}
			}
			RECT rect;
			::GetWindowRect(hwnd, &rect);
			::SetParent(     overlay, hwnd);
			::SetWindowPos(  overlay, HWND_TOP, 0, 0, (rect.right-rect.left), (rect.bottom-rect.top), 0); //SWP_HIDEWINDOW);

			::InvalidateRect(overlay, NULL, TRUE);
			::UpdateWindow(  overlay);
			::ShowWindow(    overlay, SW_SHOWNORMAL);
			drawBitmap(dc);

			::ReleaseDC(overlay, dc);
			return true;
		}
		bool initOverlay() {
			HINSTANCE hinst = ::GetModuleHandle(NULL);
			if (!hinst) return false;
			if (!WindowClass) {
				WNDCLASSEXW wcex = {
					/*size*/sizeof(WNDCLASSEX), /*style*/CS_PARENTDC|CS_VREDRAW|CS_HREDRAW, /*proc*/WndProc, /*extra*/0L,0L, /*hinst*/hinst,
					/*icon*/NULL, /*cursor*/::LoadCursor(NULL, IDC_ARROW), /*brush*/(HBRUSH)::GetStockObject(BLACK_BRUSH), /*menu*/NULL,
					/*class*/TJS_W("WindowEx OverlayBitmap Window Class"), /*smicon*/NULL };
				WindowClass = ::RegisterClassExW(&wcex);
				if (!WindowClass) return false;
			}
			if (!overlay) {
				overlay = ::CreateWindowExW(WS_EX_TOPMOST, (LPCWSTR)MAKELONG(WindowClass, 0), TJS_W("WindowExOverlayBitmap"),
											WS_CHILDWINDOW, 0, 0, 1, 1, TVPGetApplicationWindowHandle(), NULL, hinst, NULL);
				if (!overlay) return false;
				::SetWindowLong(overlay, GWL_USERDATA, (LONG)this);
			}
			return true;
		}
		void show() const { if (overlay) ::ShowWindow(overlay, SW_SHOWNORMAL); }
		void hide() const { if (overlay) ::ShowWindow(overlay, SW_HIDE);       }

	private:
		HWND overlay;
		HBITMAP bitmap;
		HDC bmpdc;
		tjs_int bmpx, bmpy, bmpw, bmph;
		static ATOM WindowClass;
	} *ovbmp;
};
ATOM WindowEx::OverlayBitmap::WindowClass = 0;

// 拡張イベント用ネイティブインスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowEx)
{
	/**/  NCB_GET_INSTANCE_HOOK_CLASS () {}
	/**/ ~NCB_GET_INSTANCE_HOOK_CLASS () {}
	NCB_INSTANCE_GETTER(objthis) {
		ClassT* obj = GetNativeInstance(objthis);
		if (!obj) SetNativeInstance(objthis, (obj = new ClassT(objthis)));
		return obj;
	}
};
// メソッド追加
NCB_ATTACH_CLASS_WITH_HOOK(WindowEx, Window)
{
	RawCallback(TJS_W("minimize"),            &Class::minimize,          0);
	RawCallback(TJS_W("maximize"),            &Class::maximize,          0);
	RawCallback(TJS_W("maximized"),           &Class::getMaximized,      &Class::setMaximized, 0);
	RawCallback(TJS_W("showRestore"),         &Class::showRestore,       0);
	RawCallback(TJS_W("resetWindowIcon"),     &Class::resetWindowIcon,   0);
	RawCallback(TJS_W("getWindowRect"),       &Class::getWindowRect,     0);
	RawCallback(TJS_W("getClientRect"),       &Class::getClientRect,     0);
	RawCallback(TJS_W("getNormalRect"),       &Class::getNormalRect,     0);
	RawCallback(TJS_W("disableResize"),       &Class::setDisableResize,  &Class::setDisableResize, 0);
	RawCallback(TJS_W("setOverlayBitmap"),    &Class::setOverlayBitmap,  0);

	Method(     TJS_W("registerExEvent"),     &Class::checkExEvents);
}

////////////////////////////////////////////////////////////////
struct MenuItemEx
{
	// メニューを取得
	static HMENU GetHMENU(iTJSDispatch2 *obj) {
		if (!obj) return NULL;
		tTJSVariant val;
		iTJSDispatch2 *global = TVPGetScriptDispatch(), *mi;
		if (global) {
			global->PropGet(0, TJS_W("MenuItem"), 0, &val, obj);
			mi = val.AsObjectNoAddRef();
			val.Clear();
			global->Release();
		} else mi = obj;
		mi->PropGet(0, TJS_W("HMENU"), 0, &val, obj);
		return (HMENU)(tjs_int)(val);
	}
	// 親メニューを取得
	static iTJSDispatch2* GetParentMenu(iTJSDispatch2 *obj) {
		tTJSVariant val;
		obj->PropGet(0, TJS_W("parent"), 0, &val, obj);
		return val.AsObjectNoAddRef();
	}
	
	// インデックスを取得
	static UINT GetIndex(iTJSDispatch2 *obj) {
		tTJSVariant val;
		obj->PropGet(0, TJS_W("index"), 0, &val, obj);
		return (UINT)val.AsInteger();
	}
	// ウィンドウを取得
	static HWND GetHWND(iTJSDispatch2 *obj) {
		if (!obj) return NULL;
		tTJSVariant val;
		obj->PropGet(0, TJS_W("root"), 0, &val, obj);
		obj = val.AsObjectNoAddRef();
		if (!obj) return NULL;
		val.Clear();
		obj->PropGet(0, TJS_W("window"), 0, &val, obj);
		obj = val.AsObjectNoAddRef();
		return obj ? WindowEx::GetHWND(obj) : NULL;
	}

	static bool getMenuItemInfo(iTJSDispatch2 *obj, HMENU &hmenu, UINT &index, MENUITEMINFO &mi, UINT mask) {
		hmenu = GetHMENU(GetParentMenu(obj));
		if (hmenu == NULL) TVPThrowExceptionMessage(TJS_W("Cannot get parent menu."));
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.fMask = mask;
		index = GetIndex(obj);
		return !!::GetMenuItemInfo(hmenu, index, TRUE, &mi);
	}

	// property rightJustify
	tTJSVariant getRightJustify() const {
		tTJSVariant r;
		if (set_rj) r = rj;
		return r;
	}
	void setRightJustify(tTJSVariant v) {
		if (v.Type() == tvtVoid) return;
		HMENU hmenu;
		UINT index;
		MENUITEMINFO mi;
		rj = !!v.AsInteger();
		set_rj = true;
		if (getMenuItemInfo(obj, hmenu, index, mi, MIIM_FTYPE)) {
			if (rj) mi.fType |= MFT_RIGHTJUSTIFY;
			else    mi.fType &= MFT_RIGHTJUSTIFY ^ (~0L);
			::SetMenuItemInfo(hmenu, index, TRUE, &mi);
			::DrawMenuBar(GetHWND(obj));
		}
	}

	// property bmpItem
	tTJSVariant getBmpItem() const {
		tTJSVariant r;
		if (set_bmp) r = bmp;
		return r;
	}
	void setBmpItem(tTJSVariant v) {
		if (v.Type() == tvtVoid) return;
		HMENU hmenu;
		UINT index;
		MENUITEMINFO mi;
		bmp = (tjs_int)v.AsInteger();
		set_bmp = true;
		if (getMenuItemInfo(obj, hmenu, index, mi, MIIM_BITMAP)) {
			mi.hbmpItem = (HBITMAP)(bmp);
			::SetMenuItemInfo(hmenu, index, TRUE, &mi);
			::DrawMenuBar(GetHWND(obj));
		}
	}

	MenuItemEx(iTJSDispatch2* _obj) : obj(_obj), set_rj(false), set_bmp(false), rj(0), bmp(0) {}
	~MenuItemEx() {}
private:
	iTJSDispatch2 *obj;
	bool set_rj, set_bmp;
	tjs_int  rj,     bmp;
};
NCB_GET_INSTANCE_HOOK(MenuItemEx)
{
	/**/  NCB_GET_INSTANCE_HOOK_CLASS () {}
	/**/ ~NCB_GET_INSTANCE_HOOK_CLASS () {}
	NCB_INSTANCE_GETTER(objthis) {
		ClassT* obj = GetNativeInstance(objthis);
		if (!obj) SetNativeInstance(objthis, (obj = new ClassT(objthis)));
		return obj;
	}
};
NCB_ATTACH_CLASS_WITH_HOOK(MenuItemEx, MenuItem)
{
	Variant(TJS_W("biSystem"),           (tjs_int)HBMMENU_SYSTEM);
	Variant(TJS_W("biRestore"),          (tjs_int)HBMMENU_MBAR_RESTORE);
	Variant(TJS_W("biMinimize"),         (tjs_int)HBMMENU_MBAR_MINIMIZE);
	Variant(TJS_W("biClose"),            (tjs_int)HBMMENU_MBAR_CLOSE);
	Variant(TJS_W("biCloseDisabled"),    (tjs_int)HBMMENU_MBAR_CLOSE_D);
	Variant(TJS_W("biMinimizeDisabled"), (tjs_int)HBMMENU_MBAR_MINIMIZE_D);
	Variant(TJS_W("biPopupClose"),       (tjs_int)HBMMENU_POPUP_CLOSE);
	Variant(TJS_W("biPopupRestore"),     (tjs_int)HBMMENU_POPUP_RESTORE);
	Variant(TJS_W("biPopupMaximize"),    (tjs_int)HBMMENU_POPUP_MAXIMIZE);
	Variant(TJS_W("biPopupMinimize"),    (tjs_int)HBMMENU_POPUP_MINIMIZE);

	Property(TJS_W("rightJustify"), &Class::getRightJustify, &Class::setRightJustify);
	Property(TJS_W("bmpItem"),      &Class::getBmpItem,      &Class::setBmpItem     );
}


////////////////////////////////////////////////////////////////
struct ConsoleEx
{
	struct SearchWork {
		ttstr name;
		HWND result;
	};
	static BOOL CALLBACK SearchWindowClass(HWND hwnd, LPARAM lp) {
		SearchWork *wk = (SearchWork*)lp;
		tjs_char name[CLASSNAME_MAX];
		::GetClassNameW(hwnd, name, CLASSNAME_MAX);
		name[CLASSNAME_MAX-1] = 0;
		if (wk->name == ttstr(name)) {
			wk->result = hwnd;
			return FALSE;
		}
		return TRUE;
	}
	static HWND GetHWND() {
		SearchWork wk = { TJS_W("TTVPConsoleForm"), NULL };
		::EnumThreadWindows(GetCurrentThreadId(), SearchWindowClass, (LPARAM)&wk);
		return wk.result;
	}

	// restoreMaximize
	static tjs_error TJS_INTF_METHOD restoreMaximize(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		HWND hwnd = GetHWND();
		bool hasWin = (hwnd != NULL);
		if ( hasWin && ::IsZoomed(hwnd)) PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		if (r) *r = hasWin;
		return TJS_S_OK;
	}
	// getPlacement
	static tjs_error TJS_INTF_METHOD getPlacement(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		HWND hwnd = GetHWND();
		WINDOWPLACEMENT place;
		ZeroMemory(&place, sizeof(place));
		place.length = sizeof(place);
		if (r) r->Clear();
		if (hwnd != NULL && ::GetWindowPlacement(hwnd, &place)) {
			ncbDictionaryAccessor dict;
			dict.SetValue(TJS_W("flags"       ), place.flags);
			dict.SetValue(TJS_W("showCmd"     ), place.showCmd);
			dict.SetValue(TJS_W("minLeft"     ), place.ptMinPosition.x);
			dict.SetValue(TJS_W("minTop"      ), place.ptMinPosition.y);
			dict.SetValue(TJS_W("maxLeft"     ), place.ptMaxPosition.x);
			dict.SetValue(TJS_W("maxTop"      ), place.ptMaxPosition.y);
			dict.SetValue(TJS_W("normalLeft"  ), place.rcNormalPosition.left);
			dict.SetValue(TJS_W("normalTop"   ), place.rcNormalPosition.top);
			dict.SetValue(TJS_W("normalRight" ), place.rcNormalPosition.right);
			dict.SetValue(TJS_W("normalBottom"), place.rcNormalPosition.bottom);
			if (r) *r = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
	}
	// setPlacement
	static tjs_error TJS_INTF_METHOD setPlacement(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		if (n < 1)return TJS_E_BADPARAMCOUNT;
		if (p[0]->Type() != tvtObject) return TJS_E_INVALIDPARAM;

		HWND hwnd = GetHWND();
		if (hwnd != NULL) {
			ncbPropAccessor dict(*p[0]);
			WINDOWPLACEMENT place;
			ZeroMemory(&place, sizeof(place));
			place.length                  = sizeof(place);
			place.flags                   = dict.getIntValue(TJS_W("flags"       ));
			place.showCmd                 = dict.getIntValue(TJS_W("showCmd"     ));
			place.ptMinPosition.x         = dict.getIntValue(TJS_W("minLeft"     ));
			place.ptMinPosition.y         = dict.getIntValue(TJS_W("minTop"      ));
			place.ptMaxPosition.x         = dict.getIntValue(TJS_W("maxLeft"     ));
			place.ptMaxPosition.y         = dict.getIntValue(TJS_W("maxTop"      ));
			place.rcNormalPosition.left   = dict.getIntValue(TJS_W("normalLeft"  ));
			place.rcNormalPosition.top    = dict.getIntValue(TJS_W("normalTop"   ));
			place.rcNormalPosition.right  = dict.getIntValue(TJS_W("normalRight" ));
			place.rcNormalPosition.bottom = dict.getIntValue(TJS_W("normalBottom"));
			if (r) *r = !!::SetWindowPlacement(hwnd, &place);
		} else if (r) *r = false;
		return TJS_S_OK;
	}
	// setPos
	static tjs_error TJS_INTF_METHOD setPos(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		HWND hwnd = GetHWND();
		if (hwnd != NULL) {
			UINT flag = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER;
			int x, y, w, h;
			x = (int)p[0]->AsInteger();
			y = (int)p[1]->AsInteger();
			if (p[0]->Type() == tvtVoid &&
				p[1]->Type() == tvtVoid) flag |= SWP_NOMOVE;
			if (n >= 4) {
				w = (int)p[2]->AsInteger();
				h = (int)p[3]->AsInteger();
				if (p[2]->Type() == tvtVoid &&
					p[3]->Type() == tvtVoid) flag |= SWP_NOSIZE;
			} else                           flag |= SWP_NOSIZE;
			::SetWindowPos(hwnd, NULL, x, y, w, h, flag);
		}
		return TJS_S_OK;
	}
};
NCB_ATTACH_FUNCTION_WITHTAG(restoreMaximize, Debug_console, Debug.console, ConsoleEx::restoreMaximize);
NCB_ATTACH_FUNCTION_WITHTAG(setPos,          Debug_console, Debug.console, ConsoleEx::setPos);
NCB_ATTACH_FUNCTION_WITHTAG(getPlacement,    Debug_console, Debug.console, ConsoleEx::getPlacement);
NCB_ATTACH_FUNCTION_WITHTAG(setPlacement,    Debug_console, Debug.console, ConsoleEx::setPlacement);


////////////////////////////////////////////////////////////////

struct System
{
	//--------------------------------------------------------------
	// ユーティリティ

	// RECTを辞書に保存
	static bool SetDictRect(ncbDictionaryAccessor &dict, tjs_char const *name, LPCRECT lprect) {
		ncbDictionaryAccessor rdic;
		if (rdic.IsValid()) {
			WindowEx::SetRect(rdic, lprect);
			dict.SetValue(name, tTJSVariant(rdic, rdic));
			return true;
		}
		return false;
	}

	// 引数からRECTを取得
	static void GetDictRect(LPRECT lprect, tTJSVariant **param) {
		lprect->left   = (LONG)param[0]->AsInteger();
		lprect->top    = (LONG)param[1]->AsInteger();
		lprect->right  = (LONG)param[2]->AsInteger();
		lprect->bottom = (LONG)param[3]->AsInteger();
		lprect->right  += lprect->left;
		lprect->bottom += lprect->top;
	}

	// モニタ情報辞書を生成
	static bool SetDictMonitorInfo(HMONITOR hMonitor, ncbDictionaryAccessor &dict) {
		MONITORINFOEXW mi;
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		if (::GetMonitorInfoW(hMonitor, &mi)) {
			dict.SetValue(TJS_W("name"),    ttstr(mi.szDevice));
			dict.SetValue(TJS_W("primary"), !!(mi.dwFlags & MONITORINFOF_PRIMARY));
			SetDictRect(dict, TJS_W("monitor"), &mi.rcMonitor);
			SetDictRect(dict, TJS_W("work"),    &mi.rcWork);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	// EnumDisplayMonitorsコールバック

	// 配列にモニタ情報辞書を追加
	static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
		ncbArrayAccessor *array = (ncbArrayAccessor*)dwData;
		ncbDictionaryAccessor dict;
		if (dict.IsValid() && SetDictMonitorInfo(hMonitor, dict)) {
			if (lprcMonitor) SetDictRect(dict, TJS_W("intersect"), lprcMonitor);
			array->SetValue(array->GetArrayCount(), tTJSVariant(dict, dict));
		}
		return TRUE;
	}

	// プライマリモニタハンドルを取得
	static BOOL CALLBACK PrimaryGetProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
		MONITORINFO mi;
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		if (::GetMonitorInfo(hMonitor, &mi) && mi.dwFlags & MONITORINFOF_PRIMARY) {
			*(HMONITOR*)dwData = hMonitor;
			return FALSE;
		}
		return TRUE;
	}

	//--------------------------------------------------------------
	// クラス追加メソッド

	// System.getDisplayMonitors
	static tjs_error TJS_INTF_METHOD getDisplayMonitors(
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		RECT rect;
		bool useRect = false;

		// パラメータ個数は 0 または 4個のみ
		switch (numparams) {
		case 0: break;
		case 4:
			GetDictRect(&rect, param);
			useRect = true;
			break;
		default: return TJS_E_BADPARAMCOUNT;
		}

		result->Clear();
		ncbArrayAccessor array;
		if (array.IsValid()) {
			if (::EnumDisplayMonitors(NULL, useRect ? &rect : NULL, &MonitorEnumProc, (LPARAM)&array))
				*result = tTJSVariant(array, array);
		}
		return TJS_S_OK;
	}

	// System.getMonitorInfo
	static tjs_error TJS_INTF_METHOD getMonitorInfo(
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		HMONITOR mon = NULL;
		DWORD flag = (numparams > 0 && param[0]->AsInteger()) ? MONITOR_DEFAULTTONEAREST : MONITOR_DEFAULTTONULL;

		// パラメータ個数によって動作変更
		switch (numparams) {
		case 0: // 省略時
			::EnumDisplayMonitors(NULL, NULL, &PrimaryGetProc, (LPARAM)&mon);
			break;
		case 2: // window指定
			iTJSDispatch2 *obj;
			if (param[1]->Type() != tvtObject) return TJS_E_INVALIDPARAM;
			obj = param[1]->AsObjectNoAddRef();
			if (obj && obj->IsInstanceOf(0, 0, 0, TJS_W("Window"), obj)) {
				HWND hwnd = WindowEx::GetHWND(obj);
				mon = ::MonitorFromWindow(hwnd, flag);
			} else return TJS_E_INVALIDPARAM;
			break;
		case 3: // 座標指定
			POINT pt;
			pt.x = (LONG)param[0]->AsInteger();
			pt.y = (LONG)param[1]->AsInteger();
			mon = ::MonitorFromPoint(pt, flag);
			break;
		case 5: // 範囲指定
			RECT rect;
			GetDictRect(&rect, param+1);
			mon = ::MonitorFromRect(&rect, flag);
			break;
		default: return TJS_E_BADPARAMCOUNT;
		}

		result->Clear();
		if (mon != NULL) {
			ncbDictionaryAccessor dict;
			if (dict.IsValid() && SetDictMonitorInfo(mon, dict))
				*result = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
	}

	// System.getMouseCursorPos
	static tjs_error TJS_INTF_METHOD getCursorPos(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		POINT pt = { 0, 0 };
		if (r) r->Clear();
		if (::GetCursorPos(&pt)) {
			ncbDictionaryAccessor dict;
			if (dict.IsValid()) {
				dict.SetValue(TJS_W("x"), pt.x);
				dict.SetValue(TJS_W("y"), pt.y);
				if (r) *r = tTJSVariant(dict, dict);
			}
		}
		return TJS_S_OK;
	}
	// System.setMouseCursorPos
	static tjs_error TJS_INTF_METHOD setCursorPos(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		if (n < 2) return TJS_E_BADPARAMCOUNT;
		BOOL rslt = ::SetCursorPos((int)p[0]->AsInteger(), (int)p[1]->AsInteger());
		if (r) {
			r->Clear();
			*r = rslt ? true : false;
		}
		return TJS_S_OK;
	}
};

// Systemに関数を追加
NCB_ATTACH_FUNCTION(getDisplayMonitors, System, System::getDisplayMonitors);
NCB_ATTACH_FUNCTION(getMonitorInfo,     System, System::getMonitorInfo);
NCB_ATTACH_FUNCTION(getCursorPos,       System, System::getCursorPos);
NCB_ATTACH_FUNCTION(setCursorPos,       System, System::setCursorPos);
