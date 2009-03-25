#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <commctrl.h>
#include "ncbind/ncbind.hpp"

#define CLASSNAME _T("WindowExProgress")
#define KRKRDISPWINDOWCLASS _T("TScrollBox")

#ifndef ID_CANCEL
#define ID_CANCEL 3
#endif

/**
 * セーブ処理スレッド用情報
 * プログレス処理を実行するウインドウ
 */
class ProgressWindow {

public:
	/**
	 * ウインドウクラスの登録
	 */
	static void registerWindowClass() {
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof wcex);
		wcex.cbSize		= sizeof(WNDCLASSEX);
		wcex.style		= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= GetModuleHandle(NULL);
		wcex.hIcon		    = NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_WAIT);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= CLASSNAME;
		wcex.hIconSm		= 0;
		RegisterClassEx(&wcex);
	}

	/**
	 * ウインドウクラスの削除
	 */
	static void unregisterWindowClass() {
		UnregisterClass(CLASSNAME, GetModuleHandle(NULL));
	}

	/**
	 * コンストラクタ
	 */
	ProgressWindow(iTJSDispatch2 *window) : window(window), hParent(0), hWnd(0), thread(0), doneflag(false), cancelflag(false), percent(0), hProgress(0){
		prepare = CreateEvent(NULL, FALSE, FALSE, NULL);
		setReceiver(true);
		start();
	}

	/**
	 * デストラクタ
	 */
	~ProgressWindow() {
		CloseHandle(prepare);
		setReceiver(false);
		end();
	}
	
	/**
	 * プログレス通知
	 * @return キャンセルされてたら true
	 */
	bool doProgress(int percent) {
		if (percent != this->percent) {
			this->percent = percent;
			if (hProgress) {
				SendMessage(hProgress, PBM_SETPOS, (WPARAM)percent, 0 );
			}
		}
		return !hWnd && cancelflag;
	}

protected:
	iTJSDispatch2 *window; //< 親ウインドウ
	HWND hParent; //< 親ハンドル
	HWND hWnd; //< 自分のハンドル
	HANDLE thread; //< プログレス処理のスレッド
	HANDLE prepare; //< 準備待ちイベント
	bool doneflag;   // 終了フラグ
	bool cancelflag; // キャンセルフラグ
	int percent; // パーセント指定

	HWND hProgress; //< プログレスバーのハンドラ
	
	/**
	 * ウインドウプロシージャ
	 */
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		ProgressWindow *self = (ProgressWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (self) {
			switch (message) {
			case WM_PAINT: // 画面更新
				{
					PAINTSTRUCT ps;
					BeginPaint(hWnd, &ps);
					self->show();
					EndPaint(hWnd, &ps);
				}
				return 0;
			case WM_COMMAND: // キャンセル通知
				switch (wParam) {
				case ID_CANCEL:
					self->cancel();
					break;
				}
				break;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	/*
	 * ウインドウイベント処理レシーバ
	 */
	static bool __stdcall receiver(void *userdata, tTVPWindowMessage *Message) {
		ProgressWindow *self = (ProgressWindow*)userdata;
		switch (Message->Msg) {
		case TVP_WM_ATTACH:
			self->start();
			break;
		case TVP_WM_DETACH:
			self->end();
			break;
		default:
			break;
		}
		return false;
	}

	// ユーザメッセージレシーバの登録/解除
	void setReceiver(bool enable) {
		tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
		tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
		tTJSVariant userdata = (tTVInteger)(tjs_int)this;
		tTJSVariant *p[] = {&mode, &proc, &userdata};
		if (window->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, window) != TJS_S_OK) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}
	
	// 実行スレッド
	static unsigned __stdcall threadFunc(void *data) {
		((ProgressWindow*)data)->main();
		_endthreadex(0);
		return 0;
	}

	/**
	 * 処理開始
	 */
	void start() {
		end();
		doneflag = false;
		tTJSVariant krkrHwnd;
		if (TJS_SUCCEEDED(window->PropGet(0, TJS_W("HWND"), NULL, &krkrHwnd, window))) {
			hParent = ::FindWindowEx((HWND)(tjs_int)krkrHwnd, NULL, KRKRDISPWINDOWCLASS, NULL);
			if (hParent) {
				thread = (HANDLE)_beginthreadex(NULL, 0, threadFunc, this, 0, NULL);
				if (thread) {
					WaitForSingleObject(prepare, 1000 * 3);
				}
			}
		}
	}
	
	/**
	 * 処理終了
	 */
	void end() {
		doneflag = true;
		if (thread) {
			WaitForSingleObject(thread, INFINITE);
			CloseHandle(thread);
			thread = 0;
		}
		hParent = 0;
	}

	/**
	 * 実行メイン処理
	 * ウインドウの生成から破棄までを独立したスレッドで行う
	 */
	void main() {
		// ウインドウ生成
		if (hParent && !hWnd) {
			RECT rect;
			POINT point;
			point.x = 0;
			point.y = 0;
			::GetClientRect(hParent, &rect);
			::ClientToScreen(hParent, &point);
			int left   = point.x;
			int top    = point.y;
			int width  = rect.right  - rect.left;
			int height = rect.bottom - rect.top;
			hWnd = ::CreateWindowEx(0, CLASSNAME, _T(""), WS_POPUP, left, top, width, height, 0, 0, GetModuleHandle(NULL), NULL);
			if (hWnd && !doneflag) {
				::SetWindowLong(hWnd, GWL_USERDATA, (LONG)this);
				::ShowWindow(hWnd,TRUE);
				create();
				// 待ち合わせ完了
				SetEvent(prepare);
				// メッセージループの実行
				MSG msg;
				while (!doneflag) {
					if (::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
						if (GetMessage(&msg, NULL, 0, 0)) {
							::TranslateMessage (&msg);
							::DispatchMessage (&msg);
						} else {
							break;
						}
					} else {
					    Sleep(0);
					}
				}
				// ウインドウの破棄
				::DestroyWindow(hWnd);
				hWnd = 0;
			}
		}
	}

	// -------------------------------------------------------------
	
	/**
	 * 描画内容生成
	 */
	void create() {

		// プログレスバーの配置決定
		RECT rect;
		GetClientRect(hWnd, &rect);
		int swidth  = rect.right  - rect.left;
		int sheight = rect.bottom - rect.top;
		int width = swidth/3;
		int height = sheight/10;
		// プログレスバーを作成
		hProgress = CreateWindowEx(0, PROGRESS_CLASS, _T(""),
								   WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
								   (swidth-width)/2, (sheight-height)/2, width, height, hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
		SendMessage(hProgress, PBM_SETRANGE , 0, MAKELPARAM(0, 100));
		SendMessage(hProgress, PBM_SETSTEP, 1, 0 );
		SendMessage(hProgress, PBM_SETPOS, percent, 0);
	}
	
	/**
	 * 画面更新処理
	 */
	void show() {
		// ビットマップ指定があれば背景をそれで塗りつぶす
	}

	/**
	 * キャンセル通知
	 */
	void cancel() {
		cancelflag = true;
	}
};

/**
 * ウインドウにレイヤセーブ機能を拡張
 */
class WindowExProgress {

protected:
	iTJSDispatch2 *objthis; //< オブジェクト情報の参照
	ProgressWindow *progressWindow; //< プログレス表示用

public:
	/**
	 * コンストラクタ
	 */
	WindowExProgress(iTJSDispatch2 *objthis) : objthis(objthis), progressWindow(NULL) {}

	/**
	 * デストラクタ
	 */
	~WindowExProgress() {
		delete progressWindow;
	}

	/**
	 * プログレス処理を開始する。
	 * 吉里吉里が実行ブロック中でも正常に表示継続します。
	 * @param init 初期化データ(辞書)
	 */
	void startProgress(tTJSVariant init) {
		if (progressWindow) {
			TVPThrowExceptionMessage(L"already running progress");
		}
		progressWindow = new ProgressWindow(objthis);
	}
	
	/**
	 * プログレス処理の経過状態を通知する。
	 * @param percent 経過状態をパーセント指定
	 * @return キャンセル要求があれば true
	 */
	bool doProgress(int percent) {
		if (!progressWindow) {
			TVPThrowExceptionMessage(L"not running progress");
		}
		return progressWindow->doProgress(percent);
	}

	/**
	 * プログレス処理を終了する。
	 */
	void endProgress() {
		if (!progressWindow) {
			TVPThrowExceptionMessage(L"not running progress");
		}
		delete progressWindow;
		progressWindow = NULL;
	}
};

//---------------------------------------------------------------------------

// インスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowExProgress)
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

NCB_ATTACH_CLASS_WITH_HOOK(WindowExProgress, Window) {
	NCB_METHOD(startProgress);
	NCB_METHOD(doProgress);
	NCB_METHOD(endProgress);
};

/**
 * 登録処理後
 */
static void PreRegistCallback()
{
	ProgressWindow::registerWindowClass();
}

/**
 * 開放処理前
 */
static void PostUnregistCallback()
{
	ProgressWindow::unregisterWindowClass();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
