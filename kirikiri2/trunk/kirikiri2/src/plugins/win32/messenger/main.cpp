#include <windows.h>
#include <tchar.h>
#include <process.h>
#include "tp_stub.h"
#include "ncbind/ncbind.hpp"
#include <map>
using namespace std;

// 吉里吉里のウインドウクラス
#define KRWINDOWCLASS _T("TTVPWindowForm")
#define WM_SHELLEXECUTED (WM_APP + 0x01)
#define KEYSIZE 256

// 確保したアトム情報
static map<ttstr,ATOM> *atoms = NULL;

// 名前からシステムグローバルアトム取得
static ATOM getAtom(const TCHAR *str)
{
	ttstr name(str);
	map<ttstr,ATOM>::const_iterator n = atoms->find(name);
	if (n != atoms->end()) {
		return n->second;
	}
	ATOM atom = GlobalAddAtom(str);
	(*atoms)[name] = atom;
	return atom;
}

static void getKey(tTJSVariant &key, ATOM atom)
{
	TCHAR buf[KEYSIZE+1];
	UINT len = GlobalGetAtomName(atom, buf, KEYSIZE);
	if (len > 0) {
		buf[len] = '\0';
		key = buf;
	}
}


//---------------------------------------------------------------------------
// メッセージ受信関数
//---------------------------------------------------------------------------
static bool __stdcall MyReceiver(void *userdata, tTVPWindowMessage *Message)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)userdata;
	switch (Message->Msg) {
	case WM_COPYDATA: // 外部からの通信
		{
			COPYDATASTRUCT *copyData = (COPYDATASTRUCT*)Message->LParam;
			tTJSVariant key;
			getKey(key, (ATOM)copyData->dwData);
			tTJSVariant msg((const tjs_char *)copyData->lpData);
			tTJSVariant *p[] = {&key, &msg};
			obj->FuncCall(0, L"onMessageReceived", NULL, NULL, 2, p, obj);
		}
		return true;
	case WM_SHELLEXECUTED: // シェル実行が終了した
		{
			tTJSVariant process = (tjs_int)(HANDLE)Message->WParam;
			tTJSVariant endCode = (tjs_int)(DWORD)Message->LParam;
			tTJSVariant *p[] = {&process, &endCode};
			obj->FuncCall(0, L"onShellExecuted", NULL, NULL, 2, p, obj);
		}
		return true;
	}
	return false;
}

/**
 * メソッド追加用クラス
 */
class WindowAdd {

protected:
	iTJSDispatch2 *objthis; //< オブジェクト情報の参照
	bool messageEnable;     //< メッセージ処理が有効か

public:
	// コンストラクタ
	WindowAdd(iTJSDispatch2 *objthis) : objthis(objthis), messageEnable(false) {}

	// デストラクタ
	~WindowAdd() {
		setMessageEnable(false);
	}

	/**
	 * メッセージ受信が有効かどうかを設定
	 * @param enable true なら有効
	 */
	void setMessageEnable(bool enable) {
		if (messageEnable != enable) {
			messageEnable = enable;
			tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
			tTJSVariant proc     = (tTVInteger)reinterpret_cast<tjs_int>(MyReceiver);
			tTJSVariant userdata = (tTVInteger)(tjs_int)objthis;
			tTJSVariant *p[3] = {&mode, &proc, &userdata};
			objthis->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 3, p, objthis);
		}
	}
	
	/**
	 * @return メッセージ受信が有効かどうか
	 */
	bool getMessageEnable() {
		return messageEnable;
	}

	// 送信メッセージ情報
	struct MsgInfo {
		HWND hWnd;
		COPYDATASTRUCT copyData;
		MsgInfo(HWND hWnd, const TCHAR *key, const tjs_char *msg) : hWnd(hWnd) {
			copyData.dwData = getAtom(key);
			copyData.cbData = (TJS_strlen(msg) + 1) * sizeof(tjs_char);
			copyData.lpData = (PVOID)msg;
		}
	};

	// メッセージ送信処理
	static BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM parent) {
		MsgInfo *info = (MsgInfo*)parent;
		TCHAR buf[100];
		GetClassName(hWnd, buf, sizeof buf);
		if (info->hWnd != hWnd && _tcscmp(buf, KRWINDOWCLASS) == 0) {
			SendMessage(hWnd, WM_COPYDATA, (WPARAM)info->hWnd, (LPARAM)&info->copyData);
		}
		return TRUE;
	}

	/**
	 * メッセージ送信
	 * @param key 識別キー
	 * @param msg メッセージ
	 */
	void sendMessage(const TCHAR *key, const tjs_char *msg) {
		tTJSVariant val;
		objthis->PropGet(0, TJS_W("HWND"), NULL, &val, objthis);
		MsgInfo info(reinterpret_cast<HWND>((tjs_int)(val)), key, msg);
		EnumWindows(enumWindowsProc, (LPARAM)&info);
	}

	// -------------------------------------------------------

	/**
	 * 実行情報
	 */
	struct ExecuteInfo {
		HANDLE process;         // 待ち対象プロセス
		iTJSDispatch2 *objthis; // this 保持用
		ExecuteInfo(HANDLE process, iTJSDispatch2 *objthis) : process(process), objthis(objthis) {};
	};
	
	/**
	 * 終了待ちスレッド
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
NCB_GET_INSTANCE_HOOK(WindowAdd)
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
NCB_ATTACH_CLASS_WITH_HOOK(WindowAdd, Window) {
	Property(L"messageEnable", &WindowAdd::getMessageEnable, &WindowAdd::setMessageEnable);
	Method(L"sendMessage", &WindowAdd::sendMessage);
	Method(L"shellExecute", &WindowAdd::shellExecute);
	Method(L"terminateProcess", &WindowAdd::terminateProcess);
}


/**
 * 登録処理前
 */
void PreRegistCallback()
{
	atoms = new map<ttstr,ATOM>;
}

/**
 * 開放処理後
 */
void PostUnregistCallback()
{
	map<ttstr,ATOM>::const_iterator i = atoms->begin();
	while (i != atoms->end()) {
		GlobalDeleteAtom(i->second);
	}
	delete atoms;
	atoms = NULL;
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
