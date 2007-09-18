#include <windows.h>
#include <tchar.h>
#include "tp_stub.h"
#include "ncbind/ncbind.hpp"
#include <map>
using namespace std;

// 吉里吉里のウインドウクラス
#define KRWINDOWCLASS _T("TTVPWindowForm")

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

//---------------------------------------------------------------------------
// メッセージ受信関数
//---------------------------------------------------------------------------
static bool __stdcall MyReceiver(void *userdata, tTVPWindowMessage *Message)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)userdata;
	if (Message->Msg == WM_COPYDATA) {
		COPYDATASTRUCT *copyData = (COPYDATASTRUCT*)Message->LParam;
		TCHAR buf[256+1];
		UINT len = GlobalGetAtomName((ATOM)copyData->dwData, buf, 256);
		tTJSVariant key;
		if (len > 0) {
			buf[len] = '\0';
			key = buf;
		}
		tTJSVariant msg((const tjs_char *)copyData->lpData);
		tTJSVariant *p[] = {&key, &msg};
		obj->FuncCall(0, L"onMessageReceived", NULL, NULL, 2, p, obj);
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
