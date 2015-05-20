
#include <windows.h>
#include <tchar.h>
#include "ncbind/ncbind.hpp"


#ifndef IDI_TVPWIN32
#define IDI_TVPWIN32 107 // [XXX]
#endif
#define WM_TASKTRAY (WM_APP + 2591)
#define IDI_MY_ICON TJS_W("tasktray icon")

class WindowTasktray {
public:
  static void registerWindowMessage(void) {
    sTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
  }

protected:
    static UINT sTaskbarRestart;

	iTJSDispatch2 *objthis; //< オブジェクト情報の参照
	bool tasktrayEnable;     //< タスクトレイが有効かどうか
    HWND hwnd;
    ttstr hint;
    tjs_int infoIcon;
    ttstr infoTitle;
    ttstr infoMsg;
    tjs_int infoTimeout;

	typedef bool (__stdcall *NativeReceiver)(iTJSDispatch2 *obj, void *userdata, tTVPWindowMessage *Message);
  
	static bool __stdcall MyReceiver(void *userdata, tTVPWindowMessage *Message) {
		iTJSDispatch2 *obj = (iTJSDispatch2*)userdata; // Window のオブジェクト
		// 吉里吉里の内部処理の関係でイベント処理中は登録破棄後でも呼ばれることがあるので
		// Window の本体オブジェクトからネイティブオブジェクトを取り直す
		WindowTasktray *self = ncbInstanceAdaptor<WindowTasktray>::GetNativeInstance(obj);
		if (self == NULL) {
			return false;
		}
		switch (Message->Msg) {
		case TVP_WM_DETACH: // ウインドウが切り離された
			break; 
		case TVP_WM_ATTACH: // ウインドウが設定された
			break;
		case WM_TASKTRAY: // 外部からの通信
			{
              tTJSVariant cursorX, cursorY;
              obj->PropGet(0 , L"cursorX", NULL, &cursorX, obj);
              obj->PropGet(0 , L"cursorY", NULL, &cursorY, obj);
              tjs_int shiftValue = 0;
              if (GetKeyState(VK_SHIFT)) shiftValue |= TVP_SS_SHIFT;
              if (GetKeyState(VK_CONTROL)) shiftValue |= TVP_SS_CTRL;
              if (GetKeyState(VK_MENU)) shiftValue |= TVP_SS_ALT;
              tTJSVariant shift = shiftValue;
              tTJSVariant button;
              switch ( Message->LParam ){
              case WM_MOUSEMOVE: {
                /* マウス移動の処理 */
                tTJSVariant *params[] = { &cursorX, &cursorY, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseMove", NULL, NULL, 3, params, obj));
              }
              case WM_LBUTTONUP: {
                /* 左ボタンが離された処理 */
                button = mbLeft;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseUp", NULL, NULL, 4, params, obj));
              }
              case WM_RBUTTONUP: {
                /* 右ボタンが離された処理 */
                button = mbRight;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseUp", NULL, NULL, 4, params, obj));
              }
              case WM_MBUTTONUP: {
                /* 中ボタンが離された処理 */
                button = mbMiddle;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseUp", NULL, NULL, 4, params, obj));
              }
              case WM_LBUTTONDOWN: {
                /* 左ボタンが押された処理 */
                button = mbLeft;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseDown", NULL, NULL, 4, params, obj));
              }
              case WM_RBUTTONDOWN: {
                /* 右ボタンが押された処理 */
                button = mbRight;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseDown", NULL, NULL, 4, params, obj));
              }
              case WM_MBUTTONDOWN: {
                /* 中ボタンが押された処理 */
                button = mbMiddle;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayMouseDown", NULL, NULL, 4, params, obj));
              }
              case WM_LBUTTONDBLCLK: {
                /* 左ダブルクリックされた処理 */
                button = mbLeft;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayDoubleClick", NULL, NULL, 4, params, obj));
              }
              case WM_RBUTTONDBLCLK: {
                /* 右ダブルクリックされた処理 */
                button = mbRight;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayDoubleClick", NULL, NULL, 4, params, obj));
              }
              case WM_MBUTTONDBLCLK: {
                /* 中ダブルクリックされた処理 */
                button = mbMiddle;
                tTJSVariant *params[] = { &cursorX, &cursorY, &button, &shift };
                return ! TJS_SUCCEEDED(Try_iTJSDispatch2_FuncCall(obj, 0, L"onTasktrayDoubleClick", NULL, NULL, 4, params, obj));
              }
              default:
                /* 上記以外の処理 */
                break;
              }
              return true;
            }
		default:
          // タスクバーが再作成された際に登録をし直す
          if (Message->Msg == sTaskbarRestart
              && self->tasktrayEnable) {
            self->registerTasktray(true);
           }
          break;
		}
		return false;
	}

	/**
	 * レシーバの登録
	 */
	void registerReceiver(bool enable) {
		// レシーバ更新
		tTJSVariant mode    = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
		tTJSVariant proc     = (tTVInteger)(tjs_int)MyReceiver;
		tTJSVariant userdata = (tTVInteger)(tjs_int)objthis;
		tTJSVariant *p[3] = {&mode, &proc, &userdata};
		int ret = objthis->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 3, p, objthis);
	}

  /**
   * タスクトレイの登録
   */
  void registerTasktray(bool enable) {
    if (enable) {
      HINSTANCE hinst = GetModuleHandle(NULL);
      HICON   r = LoadIcon(hinst, L"MAINICON"); // for krkr2
      if (!r) r = LoadIcon(hinst, MAKEINTRESOURCE(IDI_TVPWIN32)); // for krkrz
      if (!r) r = LoadIcon(hinst, IDI_APPLICATION); // for others

      NOTIFYICONDATA nid = { 0 };
      nid.cbSize = sizeof(nid);
      nid.uFlags = NIF_ICON | NIF_MESSAGE |NIF_TIP;
      nid.hWnd = hwnd;
      nid.hIcon = r;
      nid.uCallbackMessage = WM_TASKTRAY;
      wcsncpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(TCHAR), hint.c_str(), hint.length());
      for (;;) {
		if (Shell_NotifyIcon(NIM_ADD, &nid))
          // 登録できたら終わり
          break;
        // タイムアウトかどうか調べる
        if (::GetLastError() != ERROR_TIMEOUT)
          // アイコン登録エラー
          TVPThrowExceptionMessage(L"tasktray register failed.");
        // 待機
        ::Sleep(1000);
        // 登録できていないことを確認する
        if (Shell_NotifyIcon(NIM_MODIFY, &nid))
          // 登録できていた
          break;
      }
      DestroyIcon(nid.hIcon);
    } else {
      NOTIFYICONDATA nid = { 0 };
      nid.cbSize = sizeof(nid);
      nid.hWnd = hwnd;
      Shell_NotifyIcon(NIM_DELETE, &nid);
    }      
  }

  void updateTasktray(UINT flags) {
      NOTIFYICONDATA nid = { 0 };
      nid.cbSize = sizeof(nid);
      nid.uFlags = flags;
      nid.hWnd = hwnd;
      if (flags & NIF_TIP)
        wcsncpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(TCHAR), hint.c_str(), hint.length());
      if (flags & NIF_INFO) {
        wcsncpy_s(nid.szInfo, sizeof(nid.szInfo) / sizeof(TCHAR), infoMsg.c_str(), infoMsg.length());
        wcsncpy_s(nid.szInfoTitle, sizeof(nid.szInfoTitle) / sizeof(TCHAR), infoTitle.c_str(), infoTitle.length());
        nid.dwInfoFlags = infoIcon;
        nid.uTimeout = infoTimeout;
      } 
      Shell_NotifyIcon(NIM_MODIFY, &nid);
  }

public:
	// コンストラクタ
	WindowTasktray(iTJSDispatch2 *objthis) : objthis(objthis), tasktrayEnable(false) {
      tTJSVariant val;
      objthis->PropGet(0, TJS_W("HWND"), NULL, &val, objthis);
      hwnd = reinterpret_cast<HWND>((tjs_int64)(val));
    }

	// デストラクタ
	~WindowTasktray() {
		// レシーバを解放
      setTasktrayEnable(false);
	}

	/**
	 * タスクトレイが有効かどうかを設定
	 * @param enable true なら有効
	 */
	void setTasktrayEnable(bool enable) {
		if (tasktrayEnable != enable) {
			tasktrayEnable = enable;
			registerReceiver(tasktrayEnable);
            registerTasktray(tasktrayEnable);
		} 
	}

	/**
	 * @return タスクトレイが有効かどうかを取得
	 */
	bool getTasktrayEnable() {
		return tasktrayEnable;
	}

  /**
   * タスクトレイのヒントを設定する
   * @param text テキスト
   */
  void setTasktrayHint(ttstr text) {
    hint = text;
    if (tasktrayEnable)
      updateTasktray(NIF_TIP);
  }

  /**
   * タスクトレイのヒントを取得設定する
   * @return テキスト
   */
  ttstr getTasktrayHint(void) {
    return hint;
  }

  /**
   * タスクトレイのバルーンチップを設定する
   * @param icon アイコン
   * @param title タイトル
   * @param msg メッセージ
   * @param timeout タイムアウト(ms。10000～30000の間で設定する)
   */
  void popupTasktrayInfo(tjs_int icon, ttstr title, ttstr msg, tjs_int timeout) {
    infoIcon = icon;
    infoTitle = title;
    infoMsg = msg;
    infoTimeout = timeout;
    if (tasktrayEnable) 
      updateTasktray(NIF_INFO);
  }
};

UINT WindowTasktray::sTaskbarRestart;


void
PreRegistCallback()
{
  WindowTasktray::registerWindowMessage();
  TVPExecuteExpression(L"global.niifNone = 0");
  TVPExecuteExpression(L"global.niifInfo = 1");
  TVPExecuteExpression(L"global.niifWarning = 2");
  TVPExecuteExpression(L"global.niifError = 3");
}

void PostUnregistCallback()
{
  TVPExecuteScript(L"delete global[\"niifNone\"];");
  TVPExecuteScript(L"delete global[\"niifInfo\"];");
  TVPExecuteScript(L"delete global[\"niifWarning\"];");
  TVPExecuteScript(L"delete global[\"niifError\"];");
}

// インスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowTasktray)
{
	NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
		ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
		if (!obj) {
			obj = new ClassT(objthis);				// ない場合は生成する
			SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
		}
		return obj;
	}
};
    
// フックつきアタッチ
NCB_ATTACH_CLASS_WITH_HOOK(WindowTasktray, Window) {
	Property(L"tasktrayEnable", &WindowTasktray::getTasktrayEnable, &WindowTasktray::setTasktrayEnable);
	Property(L"tasktrayHint", &WindowTasktray::getTasktrayHint, &WindowTasktray::setTasktrayHint);
    Method(L"popupTasktrayInfo", &WindowTasktray::popupTasktrayInfo);
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
