#include <windows.h>
#include "ncbind.hpp"

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

////////////////////////////////////////////////////////////////

struct WindowEx
{
	//--------------------------------------------------------------
	// ユーティリティ

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

	// SYSCOMMANDを送る
	static tjs_error postSysCommand(iTJSDispatch2 *objthis, WPARAM param) {
		PostMessage(GetHWND(objthis), WM_SYSCOMMAND, param, 0);
		return TJS_S_OK;
	}

	//--------------------------------------------------------------
	// クラス追加メソッド(RawCallback形式)

	// minimize, maximize, showRestore
	static tjs_error TJS_INTF_METHOD minimize(   tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { return postSysCommand(obj, SC_MINIMIZE); }
	static tjs_error TJS_INTF_METHOD maximize(   tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { return postSysCommand(obj, SC_MAXIMIZE); }
	static tjs_error TJS_INTF_METHOD showRestore(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) { return postSysCommand(obj, SC_RESTORE);  }

	// getWindowRect
	static tjs_error TJS_INTF_METHOD getWindowRect(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		RECT rect;
		HWND hwnd = GetHWND(obj);
		r->Clear();
		if (hwnd != NULL && ::GetWindowRect(hwnd, &rect)) {
			ncbDictionaryAccessor dict;
			if (SetRect(dict, &rect)) *r = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
	}

	// getClientRect
	static tjs_error TJS_INTF_METHOD getClientRect(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *obj) {
		RECT rect;
		POINT zero = { 0, 0 };
		HWND hwnd = GetHWND(obj);
		r->Clear();
		if (hwnd != NULL && ::GetClientRect(hwnd, &rect)) {
			::ClientToScreen(hwnd, &zero);
			rect.left   += zero.x; rect.top    += zero.y;
			rect.right  += zero.x; rect.bottom += zero.y;
			ncbDictionaryAccessor dict;
			if (SetRect(dict, &rect)) *r = tTJSVariant(dict, dict);
		}
		return TJS_S_OK;
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
				ncbTypedefs::Tag<LONG> LongTypeTag;
				rect->left   = dict.GetValue(TJS_W("x"), LongTypeTag);
				rect->top    = dict.GetValue(TJS_W("y"), LongTypeTag);
				rect->right  = dict.GetValue(TJS_W("w"), LongTypeTag);
				rect->bottom = dict.GetValue(TJS_W("h"), LongTypeTag);
				rect->right  += rect->left;
				rect->bottom += rect->top;
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
		}
		return false;
	}

	// メッセージレシーバ
	static bool __stdcall receiver(void *userdata, tTVPWindowMessage *mes) {
		WindowEx *inst = (WindowEx*)userdata;
		return inst ? inst->onMessage(mes) : false;
	}

	// Message Receiver 登録・解除
	void regist(bool en) {
		tTJSVariant mode(en ? wrmRegister : wrmUnregister); 
		tTJSVariant func((tTVInteger)(&receiver));
		tTJSVariant data((tTVInteger)(this));
		tTJSVariant rslt, *params[] = { &mode, &func, &data };
		self->FuncCall(0, TJS_W("registerMessageReceiver"), 0, &rslt, 3, params, self);
	}

	// ネイティブインスタンスの生成・破棄にあわせてレシーバを登録・解除する
	WindowEx(iTJSDispatch2 *obj)
		:   self(obj),
			hasResizing(false),
			hasMoving(false),
			hasMove(false)
		{ regist(true); }

	~WindowEx() { regist(false); }

	void checkExEvents() {
		hasResizing = hasMember(EXEV_RESIZING);
		hasMoving   = hasMember(EXEV_MOVING);
		hasMove     = hasMember(EXEV_MOVE);
	}
private:
	iTJSDispatch2 *self;
	bool hasResizing, hasMoving, hasMove; //< メソッドが存在するかフラグ
};

// Windowにメソッドを追加
NCB_ATTACH_FUNCTION(minimize,      Window, WindowEx::minimize);
NCB_ATTACH_FUNCTION(maximize,      Window, WindowEx::maximize);
NCB_ATTACH_FUNCTION(showRestore,   Window, WindowEx::showRestore);
NCB_ATTACH_FUNCTION(getWindowRect, Window, WindowEx::getWindowRect);
NCB_ATTACH_FUNCTION(getClientRect, Window, WindowEx::getClientRect);

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
NCB_ATTACH_CLASS_WITH_HOOK(WindowEx, Window) {
	Method(TJS_W("registerExEvent"),  &Class::checkExEvents);
}

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
			if (param[1]->Type() == tvtObject && param[1]->IsInstanceOf(TJS_W("Window"))) {
				iTJSDispatch2 *obj = param[1]->AsObjectNoAddRef();
				if (!obj) return TJS_E_INVALIDPARAM;
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
};

// Systemに関数を追加
NCB_ATTACH_FUNCTION(getDisplayMonitors, System, System::getDisplayMonitors);
NCB_ATTACH_FUNCTION(getMonitorInfo,     System, System::getMonitorInfo);
