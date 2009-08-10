#include <windows.h>
#include <tchar.h>
#include <process.h>
#include "ncbind/ncbind.hpp"

#define WM_SHELLEXECUTED (WM_APP+1)

#define WSO_LOOPTIMEOUT 100

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

/**
 * コマンドライン呼び出し
 */
tjs_error TJS_INTF_METHOD commandExecute(
	tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	tTJSVariant vStdOut;

	// パラメータチェック
	if (numparams == 0) return TJS_E_BADPARAMCOUNT;
	if (param[0]->Type() != tvtString) return TJS_E_INVALIDPARAM;

	// コマンドライン/タイムアウト取得
	int timeout = 0;
	ttstr output, cmd(L"\""), target(param[0]->GetString());

	// 吉里吉里サーチパス上にある場合はそちらを優先
	if (TVPIsExistentStorage(target)) {
		target = TVPGetPlacedPath(target);
		TVPGetLocalName(target);
	}
	cmd += target + L"\"";
	if (numparams > 1) cmd += L" " + ttstr(param[1]->GetString());
	if (numparams > 2) timeout = (tjs_int)*param[2];

	LPWSTR cmdline = (LPWSTR)cmd.c_str();
	tjs_char const *errmes = 0;
	bool timeouted = false;

	// セキュリティ属性
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength= sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// NT系の場合はセキュリティ記述子も
	OSVERSIONINFO osv;
	ZeroMemory(&osv, sizeof(osv));
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);
	if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		ZeroMemory(&sd, sizeof(sd));
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, true, NULL, false);
		sa.lpSecurityDescriptor = &sd;
	}

	// パイプを作成
	HANDLE hOR=0, hOW=0, hIR=0, hIW=0, hEW=0;
	HANDLE hOT=0, hIT=0;
	HANDLE hPID = GetCurrentProcess();
	if (!(CreatePipe(&hOT, &hOW, &sa,0) &&
		  CreatePipe(&hIR, &hIT, &sa,0) &&
		  DuplicateHandle(hPID, hOW, hPID, &hEW, 0,  TRUE, DUPLICATE_SAME_ACCESS) &&
		  DuplicateHandle(hPID, hOT, hPID, &hOR, 0, FALSE, DUPLICATE_SAME_ACCESS) &&
		  DuplicateHandle(hPID, hIT, hPID, &hIW, 0, FALSE, DUPLICATE_SAME_ACCESS)
		  )) {
		errmes = L"can't create/duplicate pipe";
	}
	CloseHandle(hOT);
	CloseHandle(hIT);
	if (errmes) goto error;

	// 子プロセス作成
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hOW;
	si.hStdInput  = hIR;
	si.hStdError  = hEW;
	si.wShowWindow = SW_HIDE;
	if (!CreateProcessW(0, cmdline, 0, 0, TRUE, CREATE_NEW_CONSOLE, 0, 0, &si, &pi)) {
		errmes = L"can't create child process";
		goto error;
	}

	// パイプから出力を読み込み
	DWORD cnt, exit=~0L, last=GetTickCount();
	PeekNamedPipe(hOR, 0, 0, 0, &cnt, NULL);
	char buf[1024], crlf=0;
	bool rest = false;
	tjs_int32 line = 0;
	iTJSDispatch2 *array = TJSCreateArrayObject();

	while (true) {
		if (cnt > 0) {
			last = GetTickCount();
			ReadFile(hOR, buf, sizeof(buf)-1, &cnt, NULL);
			buf[cnt] = 0;
			DWORD start = 0;
			for (DWORD pos = 0; pos < cnt; pos++) {
				char ch = buf[pos];
				if (ch == '\r' || ch == '\n') {
					if (crlf == 0 || crlf == ch) {
						buf[pos] = 0;
						ttstr append(buf+start);
						output += append;
						array->PropSetByNum(TJS_MEMBERENSURE, line++, &tTJSVariant(output), array);
						output.Clear();
						buf[pos] = ch;
						crlf = 0;
					}
					if (crlf) crlf = 0;
					else crlf = ch;
					start = pos+1;
				} else {
					crlf = 0;
				}
			}
			if ((rest = (start < cnt))) {
				ttstr append(buf+start);
				output += append;
			}
			if ((int)cnt == sizeof(buf)-1) {
				PeekNamedPipe(hOR, 0, 0, 0, &cnt, NULL);
				if (cnt > 0) continue;
			}
		} else {
			if (timeout > 0 && GetTickCount() > last+timeout) {
				TerminateProcess(pi.hProcess, -1);
				errmes = L"child process timeout";
				timeouted = true;
			}
		}
		DWORD wait = WaitForSingleObject(pi.hProcess, WSO_LOOPTIMEOUT);
		if (wait == WAIT_FAILED) {
			errmes = L"child process wait failed";
			break;
		}
		PeekNamedPipe(hOR, 0, 0, 0, &cnt, NULL);
		if (cnt == 0 && wait == WAIT_OBJECT_0) {
			GetExitCodeProcess(pi.hProcess, &exit);
			break;
		}
	}
	if (rest)
		array->PropSetByNum(TJS_MEMBERENSURE, line++, &tTJSVariant(output), array);

	vStdOut.SetObject(array, array);
	array->Release();

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

error:
	CloseHandle(hOR);
	CloseHandle(hOW);
	CloseHandle(hIR);
	CloseHandle(hIW);
	CloseHandle(hEW);

	ncbDictionaryAccessor ret;
	if (ret.IsValid()) {
		ret.SetValue(L"stdout", vStdOut);
		if (errmes != 0) {
			ret.SetValue(L"status", ttstr(timeouted ? L"timeout" : L"error"));
			ret.SetValue(L"message", ttstr(errmes));
		} else {
			ret.SetValue(L"status", ttstr(L"ok"));
			ret.SetValue(L"exitcode", (tjs_int)exit);
		}
	}
	if (result != NULL) *result = ret;
	return TJS_S_OK;
}


NCB_ATTACH_FUNCTION(commandExecute, System, commandExecute);

