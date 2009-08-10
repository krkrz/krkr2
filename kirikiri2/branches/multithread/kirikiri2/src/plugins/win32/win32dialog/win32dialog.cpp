#include "ncbind.hpp"
#include <commctrl.h>

// CreateDialogIndirect のテーブル書き出し用クラス
#include "dialog.hpp"

// MessageBoxでダイアログをオーナーセンター表示にする独自フラグ
#define MB_OWNER_CENTER 0x40000000L

struct Header;
struct Items;

// BITMAPラッパ
struct Bitmap {
	typedef unsigned char PIX;
	iTJSDispatch2 *lay;
	HBITMAP bitmap;
	Bitmap(iTJSDispatch2 *layer) : lay(layer), bitmap(0) { lay->AddRef(); }
	~Bitmap() {                          removeBitmap();   lay->Release(); }
	void removeBitmap() {
		if (bitmap) ::DeleteObject(bitmap);
		bitmap = NULL;
	}
	HBITMAP createBitmap(HWND hwnd) {
		ncbPropAccessor obj(lay);
		tjs_int bmpw = obj.getIntValue(TJS_W("imageWidth"));
		tjs_int bmph = obj.getIntValue(TJS_W("imageHeight"));
		tjs_int ln = obj.getIntValue(TJS_W("mainImageBufferPitch"));
		PIX *pw, *pr = reinterpret_cast<PIX*>(obj.getIntValue(TJS_W("mainImageBuffer")));

		BITMAPINFO info;
		ZeroMemory(&info, sizeof(info));
		info.bmiHeader.biSize = sizeof(BITMAPINFO);
		info.bmiHeader.biWidth = bmpw;
		info.bmiHeader.biHeight = bmph;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 24;

		removeBitmap();
		HDC dc = GetDC(hwnd);
		if ((bitmap = CreateDIBSection(dc, (LPBITMAPINFO)&info, DIB_RGB_COLORS, (LPVOID*)&pw, NULL, 0)) != NULL) {
			for (int y = bmph-1; y >= 0; y--) {
				PIX *src = pr + (y * ln);
				PIX *dst = pw + ((bmph-1 - y) * ((bmpw*3+3) & ~3L));
				for (int n = bmpw-1; n >= 0; n--, src+=4, dst+=3) {
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];
				}
			}
		}
		ReleaseDC(hwnd, dc);
		return bitmap;
	}
};

// NMHDR アクセス用クラス
struct NotifyAccessor {
	NotifyAccessor() : ptr(0), ref(0) {}
	NotifyAccessor(NMHDR const *p) : ptr((BYTE const *)p), ref(p) {}
	tjs_int64 GetHWndFrom() const { return (tjs_int64)(ref->hwndFrom); }
	UINT      GetIdFrom()   const { return ref->idFrom; }
	UINT      GetCode()     const { return ref->code; }
	inline BYTE  GetByte( int ofs) const { return ptr[ofs]; }
	inline WORD  GetWord( int ofs) const { return ( WORD)GetByte(ofs) | ((( WORD)GetByte(ofs+1))<<8);  }
	inline DWORD GetDWord(int ofs) const { return (DWORD)GetWord(ofs) | (((DWORD)GetWord(ofs+2))<<16); }
private:
	BYTE const *ptr;
	NMHDR const* ref;
};

class WIN32Dialog {
public:
	// 型のalias
	typedef ncbPropAccessor PropT;
	typedef ncbDictionaryAccessor DictT;

	typedef tTJSVariant     VarT;
	typedef iTJSDispatch2   DspT;

	typedef DialogConfig::SizeT   SizeT;
	typedef DialogConfig::NameT   NameT;
	typedef DialogConfig::StringT StringT;
private:
	typedef ncbInstanceAdaptor<WIN32Dialog> SelfAdaptorT;
	typedef ncbInstanceAdaptor<Header>      HeadAdaptorT;
	typedef ncbInstanceAdaptor<Items>       ItemAdaptorT;
	typedef ncbInstanceAdaptor<Bitmap>      BitmapAdaptorT;

	HWND dialogHWnd;
	HICON icon;
	HMODULE resource;
	DspT *owner, *objthis;
	bool modeless;
	BYTE *buf;
	BYTE *ref;
	VarT resid;
public:
	// constructor
	WIN32Dialog(DspT *_owner = 0)
		:	dialogHWnd(0),
			icon(0),
			resource(0),
			owner(_owner),
			objthis(0),
			modeless(false),
			buf(0)
	{}

	// destructor
	virtual ~WIN32Dialog() {
		//TVPAddLog(TJS_W("# WIN32Dialog.finalize()"));

		if (modeless) close(IDCANCEL);

		if (buf) TVP_free(buf);
		buf = 0;

		if (resource) FreeLibrary(resource);
		resource = 0;
	}
	BYTE* CreateBuffer(SizeT sz) {
		// 領域確保
		ref = buf = reinterpret_cast<BYTE*>(TVP_malloc(sz + 4));
		SizeT c = 0;
		DialogTemplate::Alignment(ref, c, 4);
		return ref;
	}

	// コールバック呼び出し
	LRESULT callback(NameT event, tjs_int numparams, tTJSVariant **params) {
		DspT *obj = owner ? owner : objthis;
		if (!obj) return FALSE;

		// コールバックが存在するか
		tjs_uint32 hint = 0;
		tTJSVariant rslt;
		if (TJS_FAILED(obj->PropGet(TJS_MEMBERMUSTEXIST, event, &hint, &rslt, obj))) return FALSE;

		// 引数を渡してコールバックを呼ぶ
		rslt.Clear();
		obj->FuncCall(0, event, 0, &rslt, numparams, params, obj);
		return (rslt.AsInteger() != 0) ? TRUE : FALSE;
	}
	// 通常コールバック
	LRESULT callback(NameT event, UINT msg, WPARAM wparam, LPARAM lparam) {
		tTJSVariant vmsg((tjs_int32)msg), vwp((tjs_int32)wparam), vlp((tjs_int64)lparam);
		tTJSVariant *params[] = { &vmsg, &vwp, &vlp };
		return callback(event, 3, params);
	}
	// WM_NOTIFY用コールバック
	LRESULT callback(NameT event, WPARAM wparam, NMHDR const *nmhdr) {
		NotifyAccessor acc(nmhdr);
		tTJSVariant vwp((tjs_int32)wparam), vnm;
		// ボックス化
		ncbNativeObjectBoxing::Boxing box;
		box.operator()<NotifyAccessor&> (vnm, acc, ncbTypedefs::Tag<NotifyAccessor&>());

		tTJSVariant *params[] = { &vwp, &vnm };
		return callback(event, 2, params);
	}

	// リソース読み込み
	static tjs_error TJS_INTF_METHOD loadResource(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (param[0]->Type() == tvtString) self->_loadResource(param[0]->GetString());
		if (numparams >= 2) self->_setResource(param[1]);
		return TJS_S_OK;
	}
protected:
	// DLLのリソースを読み込む
	void _loadResource(NameT module) {
		if (!module) return;
		HMODULE oldres = resource;
		resource = LoadLibrary(module);
		if (oldres) FreeLibrary(oldres);
		if (!resource) ThrowLastError();
	}
	void _setResource(VarT *id) {
		resid.Clear();
		resid = *id;
	}

public:
	// テンプレート作成
	static tjs_error TJS_INTF_METHOD makeTemplate(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self);

	bool IsValid() const { return dialogHWnd != 0; }
	void checkDialogValid() const {
		if (!IsValid()) TVPThrowExceptionMessage(TJS_W("Dialog not opened."));
	}

	// -------------------------------------------------------------
	// 各種 Item 操作関数
	HWND GetItemHWND(int id) const {
		checkDialogValid();
		HWND ret = GetDlgItem(dialogHWnd, id);
		if (!ret) ThrowLastError();
		return ret;
	}
	// for tjs
	tjs_int64 GetItem(int id) const { return (tjs_int64)GetItemHWND(id); }

	int GetItemInt(int id) const {
		checkDialogValid();
		BOOL success = FALSE;
		int ret = (int)GetDlgItemInt(dialogHWnd, id, &success, TRUE);
		if (!success) ThrowLastError();
		return ret;
	}
	void SetItemInt(int id, int value) {
		checkDialogValid();
		if (!SetDlgItemInt(dialogHWnd, id, (UINT)value, TRUE)) ThrowLastError();
	}
	StringT GetItemText(int id) const {
		checkDialogValid();
		SizeT len = GetWindowTextLength(GetItemHWND(id));
		tjs_char *p = new tjs_char[len+1];
		GetDlgItemTextW(dialogHWnd, id, p, len+1);
		StringT ret = p;
		delete[] p;
		return ret;
	}
	void SetItemText(int id, NameT string) {
		checkDialogValid();
		if (!SetDlgItemTextW(dialogHWnd, id, string)) ThrowLastError();
	}

	void SetItemEnabled(int id, bool en) {
		EnableWindow(GetItemHWND(id), en ? TRUE : FALSE);
	}
	bool GetItemEnabled(int id) {
		return IsWindowEnabled(GetItemHWND(id)) != 0;
	}
	void SetItemFocus(int id) {
		SetFocus(GetItemHWND(id));
	}
	void SetItemPos(int id, int x, int y) {
		SetWindowPos(GetItemHWND(id), 0, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
	void SetItemSize(int id, int w, int h) {
		SetWindowPos(GetItemHWND(id), 0, 0, 0, w, h, SWP_NOMOVE|SWP_NOZORDER);
	}

	static tjs_error TJS_INTF_METHOD SetItemBitmap(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		int id = (int)param[0]->AsInteger();
		Bitmap *bmp = BitmapAdaptorT::GetNativeInstance(param[1]->AsObjectNoAddRef(), true);
		if (bmp != NULL)
			*result = self->_sendItemMessage(id, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp->createBitmap(self->dialogHWnd));
		return  TJS_S_OK;
	}

	void SetPos(int x,  int y) { if (dialogHWnd) SetWindowPos(dialogHWnd, 0, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER); }
	void SetSize(int w, int h) { if (dialogHWnd) SetWindowPos(dialogHWnd, 0, 0, 0, w, h, SWP_NOMOVE|SWP_NOZORDER); }
	long GetWidth() const {
		if (!dialogHWnd) return 0;
		RECT r;
		GetWindowRect(dialogHWnd, &r);
		return r.right - r.left;
	}
	long GetHeight() const {
		if (!dialogHWnd) return 0;
		RECT r;
		GetWindowRect(dialogHWnd, &r);
		return r.bottom - r.top;
	}

	static tjs_error TJS_INTF_METHOD sendItemMessage(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		int id   = (int)param[0]->AsInteger();
		UINT msg = (UINT)param[1]->AsInteger();
		WPARAM wp = (numparams > 2) ? (WPARAM)param[2]->AsInteger() : 0;
		LPARAM lp = (numparams > 3) ? ((param[3]->Type() == tvtString) ? (LPARAM)param[3]->GetString() : (LPARAM)param[3]->AsInteger()) : 0;
		*result = self->_sendItemMessage(id, msg, wp, lp);
		return  TJS_S_OK;
	}
protected:
	LRESULT _sendItemMessage(int id, UINT msg, WPARAM wparam, LPARAM lparam) {
		checkDialogValid();
		return SendDlgItemMessage(dialogHWnd, id, msg, wparam, lparam);
	}

	// TJS辞書⇒RECTへ値をコピー
	static inline bool _getRect(RECT &rect, PropT &prop) {
		if (!prop.IsValid()) return false;
		PropT::DefsT::Tag<LONG> tagInt;
		rect.left   = prop.GetValue(TJS_W("left"),   tagInt);
		rect.top    = prop.GetValue(TJS_W("top"),    tagInt);
		rect.right  = prop.GetValue(TJS_W("right"),  tagInt);
		rect.bottom = prop.GetValue(TJS_W("bottom"), tagInt);
		return true;
	}

public:
	VarT GetBaseUnits() const {
		VarT var;
		DictT dict;
		if (dict.IsValid()) {
			LONG units = GetDialogBaseUnits();
			dict.SetValue(TJS_W("h"), LOWORD(units));
			dict.SetValue(TJS_W("v"), HIWORD(units));
			var = dict;
		}
		return var;
	}
	VarT MapRect(VarT in) const {
		checkDialogValid();
		VarT var;
		PropT from(in);
		DictT to;
		RECT rect;
		if (_getRect(rect, from) && to.IsValid()) {
			if (!MapDialogRect(dialogHWnd, &rect)) ThrowLastError();
			to.SetValue(TJS_W("left"),   rect.left);
			to.SetValue(TJS_W("top"),    rect.top);
			to.SetValue(TJS_W("right"),  rect.right);
			to.SetValue(TJS_W("bottom"), rect.bottom);
			var = to;
		}
		return var;
	}
	bool InvalidateRect(VarT vrect, bool erase) {
		checkDialogValid();
		PropT prop(vrect);
		RECT rect;
		return (_getRect(rect, prop)) ? !!::InvalidateRect(dialogHWnd, &rect, erase) : false;
	}

	// -------------------------------------------------------------
	// open (tjs raw callback)
	static tjs_error TJS_INTF_METHOD open(VarT *result, tjs_int numparams, VarT **param, DspT *objthis) {
		WIN32Dialog *self = SelfAdaptorT::GetNativeInstance(objthis); 
		if (!self) return TJS_E_NATIVECLASSCRASH;
		self->objthis = objthis;
		*result = (tjs_int32)self->_open(numparams > 0 ? *(param[0]) : VarT());
		return TJS_S_OK;
	}

	void close(DWORD id) {
		if (dialogHWnd) {
			if (!modeless) EndDialog(dialogHWnd, id);
			else DestroyWindow(dialogHWnd);
		}
		dialogHWnd = 0;
	}

	bool getModeless() const { return modeless; }
	void setModeless(bool b) {
		if (IsValid()) TVPThrowExceptionMessage(TJS_W("Dialog is opened."));
		modeless = b;
	}
	void show(int nCmdShow) {
		if (!IsValid()) TVPThrowExceptionMessage(TJS_W("Dialog is not opened."));
		ShowWindow(dialogHWnd, nCmdShow);
	}

	// -------------------------------------------------------------
	// stubs
	virtual bool onInit(   long msg, long wp, long lp)   { return false; }
	virtual bool onCommand(long msg, long wp, long lp)   { return false; }
	virtual bool onNotify( long wp, NotifyAccessor *acc) { return false; }
	virtual bool onHScroll(long msg, long wp, long lp)   { return false; }
	virtual bool onVScroll(long msg, long wp, long lp)   { return false; }
	virtual bool onSize(   long msg, long wp, long lp)   { return false; }

protected:
	// -------------------------------------------------------------
	// ダイアログを開く
	int _open(VarT win) {
		HWND hwnd = 0;
		HINSTANCE hinst = GetModuleHandle(0);
		if (win.Type() == tvtObject) {
			DspT *obj = win.AsObjectNoAddRef();
			VarT val;
			obj->PropGet(0, TJS_W("HWND"), NULL, &val, obj);
			hwnd = (HWND)((tjs_int64)(val));
			if (!icon) icon = LoadIcon(hinst, IDI_APPLICATION);
		}
		int ret;
		LPCTSTR resname = 0;
		if (resource) resname = (resid.Type() == tvtString) ? (LPWSTR)resid.GetString() : MAKEINTRESOURCE(resid.AsInteger());

		if (!modeless) {
			ret = resource ?
				DialogBoxParamW(resource, resname, hwnd, (DLGPROC)DlgProc, (LPARAM)this) :
			/**/DialogBoxIndirectParam(hinst, (LPCDLGTEMPLATE)ref, hwnd, (DLGPROC)DlgProc, (LPARAM)this);
		} else {
			dialogHWnd = resource ?
				CreateDialogParam(resource, resname, hwnd, (DLGPROC)DlgProc, (LPARAM)this) :
			/**/CreateDialogIndirectParam(hinst, (LPCDLGTEMPLATE)ref, hwnd, (DLGPROC)DlgProc, (LPARAM)this);
			ret = dialogHWnd ? 0 : -1;
		}
		if (ret == -1) ThrowLastError();
		return ret;
	}
	// GetLastErrorのエラーメッセージを取得して投げる
	static inline void ThrowLastError() {
		LPVOID lpMessageBuffer;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPWSTR)&lpMessageBuffer, 0, NULL);
		ttstr tmp((tjs_char*)lpMessageBuffer);
		LocalFree(lpMessageBuffer);
		TVPThrowExceptionMessage(tmp.c_str());
	}

public:
	// ダイアログプロシージャ
	static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		WIN32Dialog *inst;
		switch (msg) {
		case WM_INITDIALOG:
			SetWindowLong(hwnd, DWL_USER, (LONG)lparam);
			inst = (WIN32Dialog *)lparam;
			if (inst) {
				inst->dialogHWnd = hwnd;
				if (inst->icon) SendMessage(hwnd, WM_SETICON, 0, (LPARAM)inst->icon);
				return inst->callback(TJS_W("onInit"),    msg, wparam, lparam);
			}
			break;
		case WM_SIZE:    return NormalCallback(TJS_W("onSize"   ), hwnd, msg, wparam, lparam);
		case WM_HSCROLL: return NormalCallback(TJS_W("onHScroll"), hwnd, msg, wparam, lparam);
		case WM_VSCROLL: return NormalCallback(TJS_W("onVScroll"), hwnd, msg, wparam, lparam);
		case WM_COMMAND: return NormalCallback(TJS_W("onCommand"), hwnd, msg, wparam, lparam);
		case WM_NOTIFY:
			if ((inst = (WIN32Dialog *)GetWindowLong(hwnd, DWL_USER)) != 0)
				return inst->callback(TJS_W("onNotify"), wparam, (NMHDR*)lparam);
			break;
		}
		return FALSE;
	}
	static LRESULT NormalCallback(NameT cbn, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		WIN32Dialog *inst = (WIN32Dialog *)GetWindowLong(hwnd, DWL_USER);
		return (inst != 0) ? inst->callback(cbn, msg, wparam, lparam) : FALSE;
	}

	// -------------------------------------------------------------
	// テンプレート値書き出し用
	template <typename T>
	static void SetValue(PropT *prop, NameT name, T *out) {
		tjs_uint32 hint = 0;
		if (prop->HasValue(name, &hint))
			*out = static_cast<T>(prop->GetValue(name, PropT::DefsT::Tag<tjs_int>(), 0, &hint));
	}
	static void SetSzOrd(PropT *prop, NameT name, DialogTemplate::sz_Or_Ord *out) {
		tjs_uint32 hint = 0;
		tTJSVariantType type;
		if (prop->HasValue(name, &hint, &type)) {
			if (type == tvtString) out->SetString(  prop->GetValue(name, PropT::DefsT::Tag<NameT>(),   0, &hint));
			else                   out->SetID((WORD)prop->GetValue(name, PropT::DefsT::Tag<tjs_int>(), 0, &hint));
		}
	}
	static void SetString(PropT *prop, NameT name, StringT *out) {
		tjs_uint32 hint = 0;
		if (prop->HasValue(name, &hint))
			*out = prop->GetValue(name, PropT::DefsT::Tag<NameT>(), 0, &hint);
	}

	// -------------------------------------------------------------
	// メッセージボックス表示
	static int MessageBox(iTJSDispatch2* obj, NameT text, NameT caption, UINT type) {
		bool useHook = false;
		HWND hwnd = 0;
		if (obj) {
			VarT val;
			obj->PropGet(0, TJS_W("HWND"), NULL, &val, obj);
			hwnd = (HWND)((tjs_int64)(val));
			if (!hwnd) hwnd = TVPGetApplicationWindowHandle();
		}
		if (hwnd && MessageBoxHook == 0 && (type & MB_OWNER_CENTER)) {
			MessageBoxOwnerHWND = hwnd;
			MessageBoxHook = SetWindowsHookEx(WH_CBT, MessageBoxHookProc,
											  (HINSTANCE)GetModuleHandle(0),
											  (DWORD)GetWindowThreadProcessId(TVPGetApplicationWindowHandle(), 0));
			if (MessageBoxHook != NULL) useHook = true;
			else MessageBoxOwnerHWND = 0;
		}
		int ret = ::MessageBoxW(hwnd, text, caption, (type & ~MB_OWNER_CENTER));
		if (useHook) {
			UnhookWindowsHookEx(MessageBoxHook);
			MessageBoxOwnerHWND = 0;
			MessageBoxHook = 0;
		}
		return ret;
	}
	static HHOOK MessageBoxHook;
	static HWND  MessageBoxOwnerHWND;
	static LRESULT CALLBACK MessageBoxHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
		if (nCode == HCBT_ACTIVATE && MessageBoxOwnerHWND != 0) {
			RECT win, box;
			HWND hwnd = (HWND)wParam;
			GetWindowRect(MessageBoxOwnerHWND, &win);
			GetWindowRect(hwnd,                &box);
			int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
			int w = (box.right - box.left), h = (box.bottom - box.top);
			int x = (win.left + (win.right  - win.left) / 2) - (w / 2);
			int y = (win.top  + (win.bottom - win.top)  / 2) - (h / 2);
			if ((x > -w) && (y > -h) && (x < sw) && (y < sh)) {
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				if ((x+w) > sw) x = sw-w;
				if ((y+h) > sh) y = sh-h;
				SetWindowPos(hwnd, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
			MessageBoxOwnerHWND = 0;
		}
		return CallNextHookEx(MessageBoxHook, nCode, wParam, lParam);
	}

	// -------------------------------------------------------------
	// HookEx系APIをオンデマンドで読み込み
	static bool HookExAPILoaded, HookExAPIFailed;
	typedef HHOOK   (WINAPI    *SetWindowsHookExT)(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId);
	typedef BOOL    (WINAPI *UnhookWindowsHookExT)(HHOOK hhk);
	typedef LRESULT (WINAPI      *CallNextHookExT)(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam);
	static    SetWindowsHookExT    _SetWindowsHookEx;
	static UnhookWindowsHookExT _UnhookWindowsHookEx;
	static      CallNextHookExT      _CallNextHookEx;
	static bool LoadHookExAPI() {
		if (HookExAPILoaded) return true;
		HMODULE mod = LoadLibraryW(TJS_W("user32.dll"));
		if (!mod || HookExAPIFailed) return false;
		bool failed = false;
		if (!(_SetWindowsHookEx    = (SetWindowsHookExT)   GetProcAddress(mod, GetHookAPIName("Set",   1, 1)))) failed = true;
		if (!(_UnhookWindowsHookEx = (UnhookWindowsHookExT)GetProcAddress(mod, GetHookAPIName("Unhook",1, 0)))) failed = true;
		if (!(_CallNextHookEx      = (CallNextHookExT)     GetProcAddress(mod, GetHookAPIName("Call",  0, 0)))) failed = true;
		FreeLibrary(mod);
		return (failed = HookExAPIFailed) ? false : (HookExAPILoaded = true);
	}
	static HHOOK SetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId) {
		return LoadHookExAPI() ? _SetWindowsHookEx(idHook, lpfn, hMod, dwThreadId) : NULL;
	}
	static BOOL UnhookWindowsHookEx(HHOOK hhk) {
		return LoadHookExAPI() ? _UnhookWindowsHookEx(hhk) : FALSE;
	}
	static LRESULT CallNextHookEx(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam) {
		return LoadHookExAPI() ? _CallNextHookEx(hhk, nCode, wParam, lParam) : 0;
	}
	static char *GetHookAPIName(char const *name, bool win, bool wide) {
		static char tmp[256];
		strcpy_s(tmp, sizeof(tmp), name);
		strcat_s(tmp, sizeof(tmp), win  ? "Windows" :"Next");
		strcat_s(tmp, sizeof(tmp), wide ? "HookExW" :"HookEx");
		return tmp;
	}

	// -------------------------------------------------------------
	// コモンコントロール初期化
	static void InitCommonControls() {
		::InitCommonControls();
	}
	static bool InitCommonControlsEx(DWORD icc) {
		INITCOMMONCONTROLSEX init;
		ZeroMemory(&init, sizeof(init));
		init.dwSize = sizeof(init);
		init.dwICC = icc;
		return ::InitCommonControlsEx(&init) ? true : false;
	}
};
HHOOK WIN32Dialog::MessageBoxHook = 0;
HWND  WIN32Dialog::MessageBoxOwnerHWND = 0;

bool  WIN32Dialog::HookExAPILoaded = false;
bool  WIN32Dialog::HookExAPIFailed = false;
WIN32Dialog::SetWindowsHookExT    WIN32Dialog::_SetWindowsHookEx;
WIN32Dialog::UnhookWindowsHookExT WIN32Dialog::_UnhookWindowsHookEx;
WIN32Dialog::CallNextHookExT      WIN32Dialog::_CallNextHookEx;


// ダイアログヘッダ設定クラス
struct Header : public DialogHeader {
	Header() : DialogHeader() {}

	void store(tTJSVariant elmobj) {
		ncbPropAccessor elm(elmobj);
		WIN32Dialog::SetValue( &elm, TJS_W("helpID"),      &helpID);
		WIN32Dialog::SetValue( &elm, TJS_W("exStyle"),     &exStyle);
		WIN32Dialog::SetValue( &elm, TJS_W("style"),       &style);
		WIN32Dialog::SetValue( &elm, TJS_W("x"),           &x);
		WIN32Dialog::SetValue( &elm, TJS_W("y"),           &y);
		WIN32Dialog::SetValue( &elm, TJS_W("cx"),          &cx);
		WIN32Dialog::SetValue( &elm, TJS_W("cy"),          &cy);

		WIN32Dialog::SetSzOrd( &elm, TJS_W("menu"),        &menu);
		WIN32Dialog::SetSzOrd( &elm, TJS_W("windowClass"), &windowClass);

		WIN32Dialog::SetString(&elm, TJS_W("title"),       &title);

		WIN32Dialog::SetValue( &elm, TJS_W("pointSize"),   &pointSize);
		WIN32Dialog::SetValue( &elm, TJS_W("weight"),      &weight);
		WIN32Dialog::SetValue( &elm, TJS_W("italic"),      &italic);
		WIN32Dialog::SetValue( &elm, TJS_W("charset"),     &charset);

		WIN32Dialog::SetString(&elm, TJS_W("typeFace"),    &typeFace);
	}

	void set_dlgItems(WORD n) {        dlgItems = n; }
	WORD get_dlgItems() const { return dlgItems;     }
};

struct Items : public DialogItems {
	Items() : DialogItems() {}

	void store(tTJSVariant elmobj) {
		ncbPropAccessor elm(elmobj);
		WIN32Dialog::SetValue( &elm, TJS_W("helpID"),      &helpID);
		WIN32Dialog::SetValue( &elm, TJS_W("exStyle"),     &exStyle);
		WIN32Dialog::SetValue( &elm, TJS_W("style"),       &style);
		WIN32Dialog::SetValue( &elm, TJS_W("x"),           &x);
		WIN32Dialog::SetValue( &elm, TJS_W("y"),           &y);
		WIN32Dialog::SetValue( &elm, TJS_W("cx"),          &cx);
		WIN32Dialog::SetValue( &elm, TJS_W("cy"),          &cy);
		WIN32Dialog::SetValue( &elm, TJS_W("id"),          &id);

		WIN32Dialog::SetSzOrd( &elm, TJS_W("windowClass"), &windowClass);
		WIN32Dialog::SetSzOrd( &elm, TJS_W("title"),       &title);

		// extraCount/Data は未実装につき指定できない
		//WIN32Dialog::SetValue( &elm, TJS_W("extraCount"),  &extraCount);
	}
};

// テンプレート作成
tjs_error TJS_INTF_METHOD WIN32Dialog::makeTemplate(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self)
{
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;

	// Header/Itemsインスタンスの取得とサイズの計算
	BYTE *p = 0;
	Header *head = HeadAdaptorT::GetNativeInstance(param[0]->AsObjectNoAddRef(), true);
	SizeT sz = 0;
	head->Write(p, sz);

	Items *item, **items = new Items*[numparams-1];
	for (int i = 1; i < numparams; i++) {
		items[i-1] = item = ItemAdaptorT::GetNativeInstance(param[i]->AsObjectNoAddRef(), true);
		item->Write(p, sz);
	}
	p = self->CreateBuffer(sz + 2);
	ZeroMemory(p, sz);
	sz = 0;
	head->Write(p, sz);
	for (int i = 1; i < numparams; i++)
		items[i-1]->Write(p, sz);

	delete[] items;
	return TJS_S_OK;
}



NCB_REGISTER_SUBCLASS(Header) {
	Constructor();
	Method(TJS_W("store"), &Class::store);
	Property(TJS_W("dlgItems"), &Class::get_dlgItems, &Class::set_dlgItems);
}
NCB_REGISTER_SUBCLASS(Items) {
	Constructor();
	Method(TJS_W("store"), &Class::store);
}
NCB_REGISTER_SUBCLASS(Bitmap) {
	Constructor<iTJSDispatch2*>(0);
}

NCB_REGISTER_SUBCLASS(NotifyAccessor) {
	Constructor();
	Property(TJS_W("hwndFrom"), &Class::GetHWndFrom, (int)0);
	Property(TJS_W("idFrom"),   &Class::GetIdFrom,   (int)0);
	Property(TJS_W("code"),     &Class::GetCode,     (int)0);
	Method(TJS_W("getByte"),  &Class::GetByte);
	Method(TJS_W("getWord"),  &Class::GetWord);
	Method(TJS_W("getDWord"), &Class::GetDWord);
}

#define ENUM(n) Variant(#n, (long)n, 0)

NCB_REGISTER_CLASS(WIN32Dialog) {
	NCB_SUBCLASS(Header, Header);
	NCB_SUBCLASS(Items,  Items);
	NCB_SUBCLASS(Bitmap, Bitmap);
	NCB_SUBCLASS(Notify, NotifyAccessor);

	Constructor<iTJSDispatch2*>(0);

	RawCallback(TJS_W("loadResource"), &Class::loadResource, 0);
	RawCallback(TJS_W("makeTemplate"), &Class::makeTemplate, 0);
	RawCallback(TJS_W("open"), &Class::open, 0);
	Method(TJS_W("close"),     &Class::close);

	Method(TJS_W("getItem"),         &Class::GetItem);
	Method(TJS_W("setItemInt"),      &Class::SetItemInt);
	Method(TJS_W("getItemInt"),      &Class::GetItemInt);
	Method(TJS_W("setItemText"),     &Class::SetItemText);
	Method(TJS_W("getItemText"),     &Class::GetItemText);
	Method(TJS_W("setItemEnabled"),  &Class::SetItemEnabled);
	Method(TJS_W("getItemEnabled"),  &Class::GetItemEnabled);
	Method(TJS_W("setItemFocus"),    &Class::SetItemFocus);
	Method(TJS_W("setItemPos"),      &Class::SetItemPos);
	Method(TJS_W("setItemSize"),     &Class::SetItemSize);
	RawCallback(TJS_W("setItemBitmap"),   &Class::SetItemBitmap, 0);

	RawCallback(TJS_W("sendItemMessage"), &Class::sendItemMessage, 0);

	Method(TJS_W("getBaseUnits"),    &Class::GetBaseUnits);
	Method(TJS_W("mapRect"),         &Class::MapRect);
	Method(TJS_W("invalidateRect"),  &Class::InvalidateRect);

	Method(TJS_W("setPos"),          &Class::SetPos);
	Method(TJS_W("setSize"),         &Class::SetSize);
	Property(TJS_W("width"),         &Class::GetWidth,  (int)0);
	Property(TJS_W("height"),        &Class::GetHeight, (int)0);

	Method(TJS_W("onInit"),    &Class::onInit);
	Method(TJS_W("onCommand"), &Class::onCommand);
	Method(TJS_W("onNotify"),  &Class::onNotify);
	Method(TJS_W("onHScroll"), &Class::onHScroll);
	Method(TJS_W("onVScroll"), &Class::onVScroll);
	Method(TJS_W("onSize"),    &Class::onSize);

	Method(TJS_W("show"),            &Class::show);
	Property(TJS_W("modeless"),      &Class::getModeless, &Class::setModeless);

	// 定数定義

	// Window Styles
	ENUM(WS_OVERLAPPED);
	ENUM(WS_POPUP);
	ENUM(WS_CHILD);
	ENUM(WS_MINIMIZE);
	ENUM(WS_VISIBLE);
	ENUM(WS_DISABLED);
	ENUM(WS_CLIPSIBLINGS);
	ENUM(WS_CLIPCHILDREN);
	ENUM(WS_MAXIMIZE);
	ENUM(WS_CAPTION);
	ENUM(WS_BORDER);
	ENUM(WS_DLGFRAME);
	ENUM(WS_VSCROLL);
	ENUM(WS_HSCROLL);
	ENUM(WS_SYSMENU);
	ENUM(WS_THICKFRAME);
	ENUM(WS_GROUP);
	ENUM(WS_TABSTOP);

	ENUM(WS_MINIMIZEBOX);
	ENUM(WS_MAXIMIZEBOX);

	ENUM(WS_TILED);
	ENUM(WS_ICONIC);
	ENUM(WS_SIZEBOX);
	ENUM(WS_TILEDWINDOW);

	ENUM(WS_OVERLAPPEDWINDOW);
	ENUM(WS_POPUPWINDOW);
	ENUM(WS_CHILDWINDOW);

	ENUM(WS_EX_DLGMODALFRAME);
	ENUM(WS_EX_NOPARENTNOTIFY);
	ENUM(WS_EX_TOPMOST);
	ENUM(WS_EX_ACCEPTFILES);
	ENUM(WS_EX_TRANSPARENT);
	ENUM(WS_EX_MDICHILD);
	ENUM(WS_EX_TOOLWINDOW);
	ENUM(WS_EX_WINDOWEDGE);
	ENUM(WS_EX_CLIENTEDGE);
	ENUM(WS_EX_CONTEXTHELP);
	ENUM(WS_EX_RIGHT);
	ENUM(WS_EX_LEFT);
	ENUM(WS_EX_RTLREADING);
	ENUM(WS_EX_LTRREADING);
	ENUM(WS_EX_LEFTSCROLLBAR);
	ENUM(WS_EX_RIGHTSCROLLBAR);
	ENUM(WS_EX_CONTROLPARENT);
	ENUM(WS_EX_STATICEDGE);
	ENUM(WS_EX_APPWINDOW);
	ENUM(WS_EX_OVERLAPPEDWINDOW);
	ENUM(WS_EX_PALETTEWINDOW);
#ifdef   WS_EX_LAYERED
	ENUM(WS_EX_LAYERED);
#endif
	ENUM(WS_EX_NOINHERITLAYOUT);
	ENUM(WS_EX_LAYOUTRTL);
#ifdef   WS_EX_COMPOSITED
	ENUM(WS_EX_COMPOSITED);
#endif
#ifdef   WS_EX_NOACTIVATE
	ENUM(WS_EX_NOACTIVATE);
#endif

	// Dialog Box Command IDs
	ENUM(IDOK);
	ENUM(IDCANCEL);
	ENUM(IDABORT);
	ENUM(IDRETRY);
	ENUM(IDIGNORE);
	ENUM(IDYES);
	ENUM(IDNO);
	ENUM(IDCLOSE);
	ENUM(IDHELP);
#ifdef   IDTRYAGAIN
	ENUM(IDTRYAGAIN);
#endif
#ifdef   IDCONTINUE
	ENUM(IDCONTINUE);
#endif
#ifdef   IDTIMEOUT
	ENUM(IDTIMEOUT);
#endif

	// Edit Control Styles
	ENUM(ES_LEFT);
	ENUM(ES_CENTER);
	ENUM(ES_RIGHT);
	ENUM(ES_MULTILINE);
	ENUM(ES_UPPERCASE);
	ENUM(ES_LOWERCASE);
	ENUM(ES_PASSWORD);
	ENUM(ES_AUTOVSCROLL);
	ENUM(ES_AUTOHSCROLL);
	ENUM(ES_NOHIDESEL);
	ENUM(ES_OEMCONVERT);
	ENUM(ES_READONLY);
	ENUM(ES_WANTRETURN);
	ENUM(ES_NUMBER);

	// Edit Control Notification Codes
	ENUM(EN_SETFOCUS);
	ENUM(EN_KILLFOCUS);
	ENUM(EN_CHANGE);
	ENUM(EN_UPDATE);
	ENUM(EN_ERRSPACE);
	ENUM(EN_MAXTEXT);
	ENUM(EN_HSCROLL);
	ENUM(EN_VSCROLL);
#ifdef   EN_ALIGN_LTR_EC
	ENUM(EN_ALIGN_LTR_EC);
#endif
#ifdef   EN_ALIGN_RTL_EC
	ENUM(EN_ALIGN_RTL_EC);
#endif

	// Edit control EM_SETMARGIN parameters
	ENUM(EC_LEFTMARGIN);
	ENUM(EC_RIGHTMARGIN);
	ENUM(EC_USEFONTINFO);
	// wParam of EM_GET/SETIMESTATUS
#ifdef   EMSIS_COMPOSITIONSTRING
	ENUM(EMSIS_COMPOSITIONSTRING);
#endif
	// lParam for EMSIS_COMPOSITIONSTRING
#ifdef   EIMES_GETCOMPSTRATONCE
	ENUM(EIMES_GETCOMPSTRATONCE);
#endif
#ifdef   EIMES_CANCELCOMPSTRINFOCUS
	ENUM(EIMES_CANCELCOMPSTRINFOCUS);
#endif
#ifdef   EIMES_COMPLETECOMPSTRKILLFOCUS
	ENUM(EIMES_COMPLETECOMPSTRKILLFOCUS);
#endif

	// Edit Control Messages
	ENUM(EM_GETSEL);
	ENUM(EM_SETSEL);
	ENUM(EM_GETRECT);
	ENUM(EM_SETRECT);
	ENUM(EM_SETRECTNP);
	ENUM(EM_SCROLL);
	ENUM(EM_LINESCROLL);
	ENUM(EM_SCROLLCARET);
	ENUM(EM_GETMODIFY);
	ENUM(EM_SETMODIFY);
	ENUM(EM_GETLINECOUNT);
	ENUM(EM_LINEINDEX);
	ENUM(EM_SETHANDLE);
	ENUM(EM_GETHANDLE);
	ENUM(EM_GETTHUMB);
	ENUM(EM_LINELENGTH);
	ENUM(EM_REPLACESEL);
	ENUM(EM_GETLINE);
	ENUM(EM_LIMITTEXT);
	ENUM(EM_CANUNDO);
	ENUM(EM_UNDO);
	ENUM(EM_FMTLINES);
	ENUM(EM_LINEFROMCHAR);
	ENUM(EM_SETTABSTOPS);
	ENUM(EM_SETPASSWORDCHAR);
	ENUM(EM_EMPTYUNDOBUFFER);
	ENUM(EM_GETFIRSTVISIBLELINE);
	ENUM(EM_SETREADONLY);
	ENUM(EM_SETWORDBREAKPROC);
	ENUM(EM_GETWORDBREAKPROC);
	ENUM(EM_GETPASSWORDCHAR);
	ENUM(EM_SETMARGINS);
	ENUM(EM_GETMARGINS);
	ENUM(EM_SETLIMITTEXT);
	ENUM(EM_GETLIMITTEXT);
	ENUM(EM_POSFROMCHAR);
	ENUM(EM_CHARFROMPOS);
#ifdef   EM_SETIMESTATUS
	ENUM(EM_SETIMESTATUS);
#endif
#ifdef   EM_GETIMESTATUS
	ENUM(EM_GETIMESTATUS);
#endif

	// EDITWORDBREAKPROC code values
	ENUM(WB_LEFT);
	ENUM(WB_RIGHT);
	ENUM(WB_ISDELIMITER);

	// Button Control Styles
	ENUM(BS_PUSHBUTTON);
	ENUM(BS_DEFPUSHBUTTON);
	ENUM(BS_CHECKBOX);
	ENUM(BS_AUTOCHECKBOX);
	ENUM(BS_RADIOBUTTON);
	ENUM(BS_3STATE);
	ENUM(BS_AUTO3STATE);
	ENUM(BS_GROUPBOX);
	ENUM(BS_USERBUTTON);
	ENUM(BS_AUTORADIOBUTTON);
#ifdef   BS_PUSHBOX
	ENUM(BS_PUSHBOX);
#endif
	ENUM(BS_OWNERDRAW);
#ifdef   BS_TYPEMASK
	ENUM(BS_TYPEMASK);
#endif
	ENUM(BS_LEFTTEXT);
	ENUM(BS_TEXT);
	ENUM(BS_ICON);
	ENUM(BS_BITMAP);
	ENUM(BS_LEFT);
	ENUM(BS_RIGHT);
	ENUM(BS_CENTER);
	ENUM(BS_TOP);
	ENUM(BS_BOTTOM);
	ENUM(BS_VCENTER);
	ENUM(BS_PUSHLIKE);
	ENUM(BS_MULTILINE);
	ENUM(BS_NOTIFY);
	ENUM(BS_FLAT);
	ENUM(BS_RIGHTBUTTON);

	// User Button Notification Codes
	ENUM(BN_CLICKED);
	ENUM(BN_PAINT);
	ENUM(BN_HILITE);
	ENUM(BN_UNHILITE);
	ENUM(BN_DISABLE);
	ENUM(BN_DOUBLECLICKED);
	ENUM(BN_PUSHED);
	ENUM(BN_UNPUSHED);
	ENUM(BN_DBLCLK);
	ENUM(BN_SETFOCUS);
	ENUM(BN_KILLFOCUS);

	// Button Control Messages
	ENUM(BM_GETCHECK);
	ENUM(BM_SETCHECK);
	ENUM(BM_GETSTATE);
	ENUM(BM_SETSTATE);
	ENUM(BM_SETSTYLE);
	ENUM(BM_CLICK);
	ENUM(BM_GETIMAGE);
	ENUM(BM_SETIMAGE);
#ifdef   BM_SETDONTCLICK
	ENUM(BM_SETDONTCLICK);
#endif
	ENUM(BST_UNCHECKED);
	ENUM(BST_CHECKED);
	ENUM(BST_INDETERMINATE);
	ENUM(BST_PUSHED);
	ENUM(BST_FOCUS);

	// Static Control Constants
	ENUM(SS_LEFT);
	ENUM(SS_CENTER);
	ENUM(SS_RIGHT);
	ENUM(SS_ICON);
	ENUM(SS_BLACKRECT);
	ENUM(SS_GRAYRECT);
	ENUM(SS_WHITERECT);
	ENUM(SS_BLACKFRAME);
	ENUM(SS_GRAYFRAME);
	ENUM(SS_WHITEFRAME);
	ENUM(SS_USERITEM);
	ENUM(SS_SIMPLE);
	ENUM(SS_LEFTNOWORDWRAP);
	ENUM(SS_OWNERDRAW);
	ENUM(SS_BITMAP);
	ENUM(SS_ENHMETAFILE);
	ENUM(SS_ETCHEDHORZ);
	ENUM(SS_ETCHEDVERT);
	ENUM(SS_ETCHEDFRAME);
	ENUM(SS_TYPEMASK);
#ifdef   SS_REALSIZECONTROL
	ENUM(SS_REALSIZECONTROL);
#endif
	ENUM(SS_NOPREFIX);
	ENUM(SS_NOTIFY);
	ENUM(SS_CENTERIMAGE);
	ENUM(SS_RIGHTJUST);
	ENUM(SS_REALSIZEIMAGE);
	ENUM(SS_SUNKEN);
#ifdef   SS_EDITCONTROL
	ENUM(SS_EDITCONTROL);
#endif
	ENUM(SS_ENDELLIPSIS);
	ENUM(SS_PATHELLIPSIS);
	ENUM(SS_WORDELLIPSIS);
	ENUM(SS_ELLIPSISMASK);

	// Static Control Mesages
	ENUM(STM_SETICON);
	ENUM(STM_GETICON);
	ENUM(STM_SETIMAGE);
	ENUM(STM_GETIMAGE);
	ENUM(STN_CLICKED);
	ENUM(STN_DBLCLK);
	ENUM(STN_ENABLE);
	ENUM(STN_DISABLE);
#ifdef   STM_MSGMAX
	ENUM(STM_MSGMAX);
#endif

	// Dialog Styles
	ENUM(DS_ABSALIGN);
	ENUM(DS_SYSMODAL);
	ENUM(DS_LOCALEDIT);
	ENUM(DS_SETFONT);
	ENUM(DS_MODALFRAME);
	ENUM(DS_NOIDLEMSG);
	ENUM(DS_SETFOREGROUND);
	ENUM(DS_3DLOOK);
	ENUM(DS_FIXEDSYS);
	ENUM(DS_NOFAILCREATE);
	ENUM(DS_CONTROL);
	ENUM(DS_CENTER);
	ENUM(DS_CENTERMOUSE);
	ENUM(DS_CONTEXTHELP);
	ENUM(DS_SHELLFONT);
#ifdef   DS_USEPIXELS
	ENUM(DS_USEPIXELS);
#endif

	ENUM(DM_GETDEFID);
	ENUM(DM_SETDEFID);
	ENUM(DM_REPOSITION);
	// Returned in HIWORD() of DM_GETDEFID result if msg is supported
	ENUM(DC_HASDEFID);

	// Dialog Codes
	ENUM(DLGC_WANTARROWS);
	ENUM(DLGC_WANTTAB);
	ENUM(DLGC_WANTALLKEYS);
	ENUM(DLGC_WANTMESSAGE);
	ENUM(DLGC_HASSETSEL);
	ENUM(DLGC_DEFPUSHBUTTON);
	ENUM(DLGC_UNDEFPUSHBUTTON);
	ENUM(DLGC_RADIOBUTTON);
	ENUM(DLGC_WANTCHARS);
	ENUM(DLGC_STATIC);
	ENUM(DLGC_BUTTON);

	// Listbox Return Values
	ENUM(LB_CTLCODE);
	ENUM(LB_OKAY);
	ENUM(LB_ERR);
	ENUM(LB_ERRSPACE);

	// Listbox Notification Codes
	ENUM(LBN_ERRSPACE);
	ENUM(LBN_SELCHANGE);
	ENUM(LBN_DBLCLK);
	ENUM(LBN_SELCANCEL);
	ENUM(LBN_SETFOCUS);
	ENUM(LBN_KILLFOCUS);

	ENUM(LB_ADDSTRING);
	ENUM(LB_INSERTSTRING);
	ENUM(LB_DELETESTRING);
	ENUM(LB_SELITEMRANGEEX);
	ENUM(LB_RESETCONTENT);
	ENUM(LB_SETSEL);
	ENUM(LB_SETCURSEL);
	ENUM(LB_GETSEL);
	ENUM(LB_GETCURSEL);
	ENUM(LB_GETTEXT);
	ENUM(LB_GETTEXTLEN);
	ENUM(LB_GETCOUNT);
	ENUM(LB_SELECTSTRING);
	ENUM(LB_DIR);
	ENUM(LB_GETTOPINDEX);
	ENUM(LB_FINDSTRING);
	ENUM(LB_GETSELCOUNT);
	ENUM(LB_GETSELITEMS);
	ENUM(LB_SETTABSTOPS);
	ENUM(LB_GETHORIZONTALEXTENT);
	ENUM(LB_SETHORIZONTALEXTENT);
	ENUM(LB_SETCOLUMNWIDTH);
	ENUM(LB_ADDFILE);
	ENUM(LB_SETTOPINDEX);
	ENUM(LB_GETITEMRECT);
	ENUM(LB_GETITEMDATA);
	ENUM(LB_SETITEMDATA);
	ENUM(LB_SELITEMRANGE);
	ENUM(LB_SETANCHORINDEX);
	ENUM(LB_GETANCHORINDEX);
	ENUM(LB_SETCARETINDEX);
	ENUM(LB_GETCARETINDEX);
	ENUM(LB_SETITEMHEIGHT);
	ENUM(LB_GETITEMHEIGHT);
	ENUM(LB_FINDSTRINGEXACT);
	ENUM(LB_SETLOCALE);
	ENUM(LB_GETLOCALE);
	ENUM(LB_SETCOUNT);
	ENUM(LB_INITSTORAGE);
	ENUM(LB_ITEMFROMPOINT);
#ifdef   LB_MULTIPLEADDSTRING
	ENUM(LB_MULTIPLEADDSTRING);
#endif
#ifdef   LB_GETLISTBOXINFO
	ENUM(LB_GETLISTBOXINFO);
#endif
#ifdef   LB_MSGMAX
	ENUM(LB_MSGMAX);
#endif

	// Listbox Styles
	ENUM(LBS_NOTIFY);
	ENUM(LBS_SORT);
	ENUM(LBS_NOREDRAW);
	ENUM(LBS_MULTIPLESEL);
	ENUM(LBS_OWNERDRAWFIXED);
	ENUM(LBS_OWNERDRAWVARIABLE);
	ENUM(LBS_HASSTRINGS);
	ENUM(LBS_USETABSTOPS);
	ENUM(LBS_NOINTEGRALHEIGHT);
	ENUM(LBS_MULTICOLUMN);
	ENUM(LBS_WANTKEYBOARDINPUT);
	ENUM(LBS_EXTENDEDSEL);
	ENUM(LBS_DISABLENOSCROLL);
	ENUM(LBS_NODATA);
	ENUM(LBS_NOSEL);
#ifdef   LBS_COMBOBOX
	ENUM(LBS_COMBOBOX);
#endif
	ENUM(LBS_STANDARD);

	// Combo Box return Values
	ENUM(CB_OKAY);
	ENUM(CB_ERR);
	ENUM(CB_ERRSPACE);

	// Combo Box Notification Codes
	ENUM(CBN_ERRSPACE);
	ENUM(CBN_SELCHANGE);
	ENUM(CBN_DBLCLK);
	ENUM(CBN_SETFOCUS);
	ENUM(CBN_KILLFOCUS);
	ENUM(CBN_EDITCHANGE);
	ENUM(CBN_EDITUPDATE);
	ENUM(CBN_DROPDOWN);
	ENUM(CBN_CLOSEUP);
	ENUM(CBN_SELENDOK);
	ENUM(CBN_SELENDCANCEL);

	// Combo Box styles
	ENUM(CBS_SIMPLE);
	ENUM(CBS_DROPDOWN);
	ENUM(CBS_DROPDOWNLIST);
	ENUM(CBS_OWNERDRAWFIXED);
	ENUM(CBS_OWNERDRAWVARIABLE);
	ENUM(CBS_AUTOHSCROLL);
	ENUM(CBS_OEMCONVERT);
	ENUM(CBS_SORT);
	ENUM(CBS_HASSTRINGS);
	ENUM(CBS_NOINTEGRALHEIGHT);
	ENUM(CBS_DISABLENOSCROLL);
	ENUM(CBS_UPPERCASE);
	ENUM(CBS_LOWERCASE);

	// Combo Box messages
	ENUM(CB_GETEDITSEL);
	ENUM(CB_LIMITTEXT);
	ENUM(CB_SETEDITSEL);
	ENUM(CB_ADDSTRING);
	ENUM(CB_DELETESTRING);
	ENUM(CB_DIR);
	ENUM(CB_GETCOUNT);
	ENUM(CB_GETCURSEL);
	ENUM(CB_GETLBTEXT);
	ENUM(CB_GETLBTEXTLEN);
	ENUM(CB_INSERTSTRING);
	ENUM(CB_RESETCONTENT);
	ENUM(CB_FINDSTRING);
	ENUM(CB_SELECTSTRING);
	ENUM(CB_SETCURSEL);
	ENUM(CB_SHOWDROPDOWN);
	ENUM(CB_GETITEMDATA);
	ENUM(CB_SETITEMDATA);
	ENUM(CB_GETDROPPEDCONTROLRECT);
	ENUM(CB_SETITEMHEIGHT);
	ENUM(CB_GETITEMHEIGHT);
	ENUM(CB_SETEXTENDEDUI);
	ENUM(CB_GETEXTENDEDUI);
	ENUM(CB_GETDROPPEDSTATE);
	ENUM(CB_FINDSTRINGEXACT);
	ENUM(CB_SETLOCALE);
	ENUM(CB_GETLOCALE);
	ENUM(CB_GETTOPINDEX);
	ENUM(CB_SETTOPINDEX);
	ENUM(CB_GETHORIZONTALEXTENT);
	ENUM(CB_SETHORIZONTALEXTENT);
	ENUM(CB_GETDROPPEDWIDTH);
	ENUM(CB_SETDROPPEDWIDTH);
	ENUM(CB_INITSTORAGE);
#ifdef   CB_MULTIPLEADDSTRING
	ENUM(CB_MULTIPLEADDSTRING);
#endif
#ifdef   CB_GETCOMBOBOXINFO
	ENUM(CB_GETCOMBOBOXINFO);
#endif
#ifdef   CB_MSGMAX
	ENUM(CB_MSGMAX);
#endif

	// Scroll Bar Styles
	ENUM(SBS_HORZ);
	ENUM(SBS_VERT);
	ENUM(SBS_TOPALIGN);
	ENUM(SBS_LEFTALIGN);
	ENUM(SBS_BOTTOMALIGN);
	ENUM(SBS_RIGHTALIGN);
	ENUM(SBS_SIZEBOXTOPLEFTALIGN);
	ENUM(SBS_SIZEBOXBOTTOMRIGHTALIGN);
	ENUM(SBS_SIZEBOX);
	ENUM(SBS_SIZEGRIP);

	// Scroll bar messages
	ENUM(SBM_SETPOS);
	ENUM(SBM_GETPOS);
	ENUM(SBM_SETRANGE);
	ENUM(SBM_SETRANGEREDRAW);
	ENUM(SBM_GETRANGE);
	ENUM(SBM_ENABLE_ARROWS);
	ENUM(SBM_SETSCROLLINFO);
	ENUM(SBM_GETSCROLLINFO);
#ifdef   SBM_GETSCROLLBARINFO
	ENUM(SBM_GETSCROLLBARINFO);
#endif
	ENUM(SIF_RANGE);
	ENUM(SIF_PAGE);
	ENUM(SIF_POS);
	ENUM(SIF_DISABLENOSCROLL);
	ENUM(SIF_TRACKPOS);
	ENUM(SIF_ALL);

	// Font Weights
	ENUM(FW_DONTCARE);
	ENUM(FW_THIN);
	ENUM(FW_EXTRALIGHT);
	ENUM(FW_LIGHT);
	ENUM(FW_NORMAL);
	ENUM(FW_MEDIUM);
	ENUM(FW_SEMIBOLD);
	ENUM(FW_BOLD);
	ENUM(FW_EXTRABOLD);
	ENUM(FW_HEAVY);

	// ShowWindow options
	ENUM(SW_HIDE);
	ENUM(SW_SHOWNORMAL);
	ENUM(SW_NORMAL);
	ENUM(SW_SHOWMINIMIZED);
	ENUM(SW_SHOWMAXIMIZED);
	ENUM(SW_MAXIMIZE);
	ENUM(SW_SHOWNOACTIVATE);
	ENUM(SW_SHOW);
	ENUM(SW_MINIMIZE);
	ENUM(SW_SHOWMINNOACTIVE);
	ENUM(SW_SHOWNA);
	ENUM(SW_RESTORE);
	ENUM(SW_SHOWDEFAULT);
	ENUM(SW_FORCEMINIMIZE);

	// Control classes
	Variant(TJS_W("BUTTON"),    0x0080, 0);
	Variant(TJS_W("EDIT"),      0x0081, 0);
	Variant(TJS_W("STATIC"),    0x0082, 0);
	Variant(TJS_W("LISTBOX"),   0x0083, 0);
	Variant(TJS_W("SCROLLBAR"), 0x0084, 0);
	Variant(TJS_W("COMBOBOX"),  0x0085, 0);

	// for MessageBox
	ENUM(MB_ABORTRETRYIGNORE);
	ENUM(MB_CANCELTRYCONTINUE);
	ENUM(MB_HELP);
	ENUM(MB_OK);
	ENUM(MB_OKCANCEL);
	ENUM(MB_RETRYCANCEL);
	ENUM(MB_YESNO);
	ENUM(MB_YESNOCANCEL);
	ENUM(MB_ICONEXCLAMATION);
	ENUM(MB_ICONWARNING);
	ENUM(MB_ICONINFORMATION);
	ENUM(MB_ICONASTERISK);
	ENUM(MB_ICONQUESTION);
	ENUM(MB_ICONSTOP);
	ENUM(MB_ICONERROR);
	ENUM(MB_ICONHAND);
	ENUM(MB_DEFBUTTON1);
	ENUM(MB_DEFBUTTON2);
	ENUM(MB_DEFBUTTON3);
	ENUM(MB_DEFBUTTON4);
	ENUM(MB_APPLMODAL);
	ENUM(MB_SYSTEMMODAL);
	ENUM(MB_TASKMODAL);
	ENUM(MB_DEFAULT_DESKTOP_ONLY);
	ENUM(MB_RIGHT);
	ENUM(MB_RTLREADING);
	ENUM(MB_SETFOREGROUND);
	ENUM(MB_TOPMOST);
#ifdef   MB_SERVICE_NOTIFICATION
	ENUM(MB_SERVICE_NOTIFICATION);
#endif
#ifdef   MB_SERVICE_NOTIFICATION_NT3X
	ENUM(MB_SERVICE_NOTIFICATION_NT3X);
#endif

	ENUM(MB_OWNER_CENTER);

	Method(TJS_W("messageBox"), &Class::MessageBox);


	// InitCommonControlsEx parameters
	ENUM(ICC_LISTVIEW_CLASSES);
	ENUM(ICC_TREEVIEW_CLASSES);
	ENUM(ICC_BAR_CLASSES);
	ENUM(ICC_TAB_CLASSES);
	ENUM(ICC_UPDOWN_CLASS);
	ENUM(ICC_PROGRESS_CLASS);
	ENUM(ICC_HOTKEY_CLASS);
	ENUM(ICC_ANIMATE_CLASS);
	ENUM(ICC_WIN95_CLASSES);
	ENUM(ICC_DATE_CLASSES);
	ENUM(ICC_USEREX_CLASSES);
	ENUM(ICC_COOL_CLASSES);
#ifdef   ICC_INTERNET_CLASSES
	ENUM(ICC_INTERNET_CLASSES);
	ENUM(ICC_PAGESCROLLER_CLASS);
	ENUM(ICC_NATIVEFNTCTL_CLASS);
#endif
#ifdef   ICC_STANDARD_CLASSES
	ENUM(ICC_STANDARD_CLASSES);
	ENUM(ICC_LINK_CLASS);
#endif

	Variant(TJS_W("HEADER"),         WC_HEADERW, 0);			// "SysHeader32"
#ifdef WC_LINK
	Variant(TJS_W("LINK"),           WC_LINK, 0);				// "SysLink"
#endif
	Variant(TJS_W("LISTVIEW"),       WC_LISTVIEWW, 0);			// "SysListView32"
	Variant(TJS_W("TREEVIEW"),       WC_TREEVIEWW, 0);			// "SysTreeView32"
	Variant(TJS_W("TABCONTROL"),     WC_TABCONTROLW, 0);		// "SysTabControl32"
	Variant(TJS_W("IPADDRESS"),      WC_IPADDRESSW, 0);			// "SysIPAddress32"
	Variant(TJS_W("PAGESCROLLER"),   WC_PAGESCROLLERW, 0);		// "SysPager"
	Variant(TJS_W("ANIMATE"),        ANIMATE_CLASSW, 0);		// "SysAnimate32"
	Variant(TJS_W("MONTHCAL"),       MONTHCAL_CLASSW, 0);		// "SysMonthCal32"
	Variant(TJS_W("DATETIMEPICK"),   DATETIMEPICK_CLASSW, 0);	// "SysDateTimePick32"

	Variant(TJS_W("COMBOBOXEX"),     WC_COMBOBOXEXW, 0);		// "ComboBoxEx32"
	Variant(TJS_W("NATIVEFONTCTL"),  WC_NATIVEFONTCTLW, 0);		// "NativeFontCtl"
	Variant(TJS_W("TOOLBAR"),        TOOLBARCLASSNAMEW, 0);		// "ToolbarWindow32"
	Variant(TJS_W("REBAR"),          REBARCLASSNAMEW, 0);		// "ReBarWindow32"
	Variant(TJS_W("TOOLTIPS"),       TOOLTIPS_CLASSW, 0);		// "tooltips_class32"

	Variant(TJS_W("STATUS"),         STATUSCLASSNAMEW, 0);		// "msctls_statusbar32"
	Variant(TJS_W("TRACKBAR"),       TRACKBAR_CLASSW, 0);		// "msctls_trackbar32"
	Variant(TJS_W("UPDOWN"),         UPDOWN_CLASSW, 0);			// "msctls_updown32"
	Variant(TJS_W("PROGRESS"),       PROGRESS_CLASSW, 0);		// "msctls_progress32"
	Variant(TJS_W("HOTKEY"),         HOTKEY_CLASSW, 0);			// "msctls_hotkey32"

	// [XXX] コモンコントロールのメッセージ用ENUMが必要

	// 取り急ぎトラックバーのみ
	ENUM(TBS_AUTOTICKS);
	ENUM(TBS_VERT);
	ENUM(TBS_HORZ);
	ENUM(TBS_TOP);
	ENUM(TBS_BOTTOM);
	ENUM(TBS_LEFT);
	ENUM(TBS_RIGHT);
	ENUM(TBS_BOTH);
	ENUM(TBS_NOTICKS);
	ENUM(TBS_ENABLESELRANGE);
	ENUM(TBS_FIXEDLENGTH);
	ENUM(TBS_NOTHUMB);
#ifdef   TBS_TOOLTIPS
	ENUM(TBS_TOOLTIPS);
#endif
#ifdef   TBS_REVERSED
	ENUM(TBS_REVERSED);
#endif
#ifdef   TBS_DOWNISLEFT
	ENUM(TBS_DOWNISLEFT);
#endif

#ifdef   TBS_NOTIFYBEFOREMOVE
	ENUM(TBS_NOTIFYBEFOREMOVE);
#endif
#ifdef   TBS_TRANSPARENTBKGND
	ENUM(TBS_TRANSPARENTBKGND);
#endif
	ENUM(TBM_GETPOS);
	ENUM(TBM_GETRANGEMIN);
	ENUM(TBM_GETRANGEMAX);
	ENUM(TBM_GETTIC);
	ENUM(TBM_SETTIC);
	ENUM(TBM_SETPOS);
	ENUM(TBM_SETRANGE);
	ENUM(TBM_SETRANGEMIN);
	ENUM(TBM_SETRANGEMAX);
	ENUM(TBM_CLEARTICS);
	ENUM(TBM_SETSEL);
	ENUM(TBM_SETSELSTART);
	ENUM(TBM_SETSELEND);
	ENUM(TBM_GETPTICS);
	ENUM(TBM_GETTICPOS);
	ENUM(TBM_GETNUMTICS);
	ENUM(TBM_GETSELSTART);
	ENUM(TBM_GETSELEND);
	ENUM(TBM_CLEARSEL);
	ENUM(TBM_SETTICFREQ);
	ENUM(TBM_SETPAGESIZE);
	ENUM(TBM_GETPAGESIZE);
	ENUM(TBM_SETLINESIZE);
	ENUM(TBM_GETLINESIZE);
	ENUM(TBM_GETTHUMBRECT);
	ENUM(TBM_GETCHANNELRECT);
	ENUM(TBM_SETTHUMBLENGTH);
	ENUM(TBM_GETTHUMBLENGTH);
#ifdef   TBM_SETTOOLTIPS
	ENUM(TBM_SETTOOLTIPS);
	ENUM(TBM_GETTOOLTIPS);
	ENUM(TBM_SETTIPSIDE);
	// TrackBar Tip Side flags
	ENUM(TBTS_TOP);
	ENUM(TBTS_LEFT);
	ENUM(TBTS_BOTTOM);
	ENUM(TBTS_RIGHT);

	ENUM(TBM_SETBUDDY);
	ENUM(TBM_GETBUDDY);
#endif
#ifdef   TBM_SETUNICODEFORMAT
	ENUM(TBM_SETUNICODEFORMAT);
	ENUM(TBM_GETUNICODEFORMAT);
#endif
	ENUM(TB_LINEUP);
	ENUM(TB_LINEDOWN);
	ENUM(TB_PAGEUP);
	ENUM(TB_PAGEDOWN);
	ENUM(TB_THUMBPOSITION);
	ENUM(TB_THUMBTRACK);
	ENUM(TB_TOP);
	ENUM(TB_BOTTOM);
	ENUM(TB_ENDTRACK);
#ifdef   TBCD_TICS
	ENUM(TBCD_TICS);
	ENUM(TBCD_THUMB);
	ENUM(TBCD_CHANNEL);
#endif
#ifdef   TRBN_THUMBPOSCHANGING
	ENUM(TRBN_THUMBPOSCHANGING);
#endif

	Method(TJS_W("initCommonControls"),   &Class::InitCommonControls);
	Method(TJS_W("initCommonControlsEx"), &Class::InitCommonControlsEx);
}

