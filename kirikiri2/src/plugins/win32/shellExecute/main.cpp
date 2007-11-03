#include <windows.h>
#include <tchar.h>
#include <process.h>
#include "ncbind/ncbind.hpp"

#define WM_SHELLEXECUTED (WM_APP+1)

/**
 * メソッド追加用クラス
 */
class WindowShell {

protected:
	iTJSDispatch2 *objthis; //< オブジェクト情報の参照

	// イベント処理
	static bool __stdcall shellExecuted(void *userdata, tTVPWindowMessage *Message) {
		if (Message->Msg == WM_SHELLEXECUTED) {
			iTJSDispatch2 *obj = (iTJSDispatch2*)userdata;
			tTJSVariant process = (tjs_int)Message->WParam;
			tTJSVariant endCode = (tjs_int)Message->LParam;
			tTJSVariant *p[] = {&process, &endCode};
			obj->FuncCall(0, L"onShellExecuted", NULL, NULL, 2, p, obj);
			return true;
		}
		return false;
	}
	
	// ユーザメッセージレシーバの登録/解除
	void setReceiver(tTVPWindowMessageReceiver receiver, bool enable) {
		tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
		tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
		tTJSVariant userdata = (tTVInteger)(tjs_int)objthis;
		tTJSVariant *p[] = {&mode, &proc, &userdata};
		if (objthis->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, objthis) != TJS_S_OK) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}
	
public:
	// コンストラクタ
	WindowShell(iTJSDispatch2 *objthis) : objthis(objthis) {
		setReceiver(shellExecuted, true);
	}

	// デストラクタ
	~WindowShell() {
		setReceiver(shellExecuted, false);
	}

public:
	/**
	 * 実行情報
	 */
	struct ExecuteInfo {
		HANDLE process;         // 待ち対象プロセス
		iTJSDispatch2 *objthis; // this 保持用
		ExecuteInfo(HANDLE process, iTJSDispatch2 *objthis) : process(process), objthis(objthis) {};
	};
	
	/**
	 * 終了待ちスレッド処理
	 * @param data ユーザ(ExecuteInfo)
	 */
	static void waitProcess(void *data) {
		// パラメータ引き継ぎ
		HANDLE process         = ((ExecuteInfo*)data)->process;
		iTJSDispatch2 *objthis = ((ExecuteInfo*)data)->objthis;
		delete data;

		// プロセス待ち
		WaitForSingleObject(process, INFINITE);
		DWORD dt;
		GetExitCodeProcess(process, &dt); // 結果取得
		CloseHandle(process);

		// 送信
		tTJSVariant val;
		objthis->PropGet(0, TJS_W("HWND"), NULL, &val, objthis);
		PostMessage(reinterpret_cast<HWND>((tjs_int)(val)), WM_SHELLEXECUTED, (WPARAM)process, (LPARAM)dt);
	}
	
	/**
	 * プロセスの停止
	 * @param keyName 識別キー
	 * @param endCode 終了コード
	 */
	void terminateProcess(int process, int endCode) {
		TerminateProcess((HANDLE)process, endCode);
	}

	/**
	 * プロセスの実行
	 * @param keyName 識別キー
	 * @param target ターゲット
	 * @praam param パラメータ
	 */
	int shellExecute(LPCTSTR target, LPCTSTR param) {
		SHELLEXECUTEINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(si);
		si.lpVerb = _T("open");
		si.lpFile = target;
		si.lpParameters = param;
		si.nShow = SW_SHOWNORMAL;
		si.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
		if (ShellExecuteEx(&si)) {
			_beginthread(waitProcess, 0, new ExecuteInfo(si.hProcess, objthis));
			return (int)si.hProcess;
		}
		return (int)INVALID_HANDLE_VALUE;
	}

};

// インスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowShell)
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
NCB_ATTACH_CLASS_WITH_HOOK(WindowShell, Window) {
	Method(L"shellExecute", &WindowShell::shellExecute);
	Method(L"terminateProcess", &WindowShell::terminateProcess);
}
