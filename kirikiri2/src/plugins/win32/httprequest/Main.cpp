#include "HttpConnection.h"
#include "ncbind/ncbind.hpp"
#include <vector>
using namespace std;
#include <process.h>

// メッセージコード
#define	WM_HTTP_READYSTATE	(WM_APP+6)	// ステート変更
#define	WM_HTTP_PROGRESS	(WM_APP+7)	// プログレス状態

// エージェント名
#define AGENT_NAME			_T("KIRIKIRI")

// Content-Type 取得用 RegExp インスタンス
static tTJSVariant *regctype = NULL;

bool
matchContentType(const BYTE *text, tstring &ctype)
{
	bool ret = false;
	
	tTJSVariant t(ttstr((const tjs_nchar*)text));
	tTJSVariant *params[] = { &t };
	tTJSVariant matchResult;
	if (TJS_SUCCEEDED(regctype->AsObjectClosureNoAddRef().FuncCall(0, TJS_W("match"), NULL, &matchResult, 1, params, NULL))) {
		tTJSVariant matchStr;
		if (TJS_SUCCEEDED(matchResult.AsObjectClosureNoAddRef().PropGetByNum(0, 2, &matchStr, NULL))) {
			tTJSVariantString *str = matchStr.AsString();
			if (str) {
				int len = str->GetLength();
				const tjs_char *buf = *str;
				if (len > 0) {
					if (buf[0] == '\'' || buf[0] == '"') {
						ctype = tstring(buf+1, len-2);
					} else {
						ctype = tstring(buf, len);
					}
					ret = true;
				}
				str->Release();
			}
		}
	}
	return ret;
}

/**
 * HttpRequest クラス
 */
class HttpRequest {

public:

	enum ReadyState {
		READYSTATE_UNINITIALIZED,
		READYSTATE_OPEN,
		READYSTATE_SENT,
		READYSTATE_RECEIVING,
		READYSTATE_LOADED
	};

	/**
	 * コンストラクタ
	 * @param objthis 自己オブジェクト
	 * @param window 親ウインドウ
	 * @param cert HTTP通信時に証明書チェックを行う
	 */
	HttpRequest(iTJSDispatch2 *objthis, iTJSDispatch2 *window, bool cert, const tjs_char *agentName)
		 : objthis(objthis), window(window), http(agentName, cert),
		   threadHandle(NULL), canceled(false),
		   output(NULL), readyState(READYSTATE_UNINITIALIZED), statusCode(0)
	{
		window->AddRef();
		setReceiver(true);
	}
	
	/**
	 * デストラクタ
	 */
	~HttpRequest() {
		abort();
		setReceiver(false);
		window->Release();
	}

	/**
	 * 指定したメソッドで指定URLにリクエストする
	 * ※常に非同期での呼び出しになります
	 * @param method GET|PUT|POST のいずれか
	 * @param url リクエスト先のURL
	 * @param userName ユーザ名。指定すると認証ヘッダをつけます
	 * @param password パスワード
	 */
	void _open(const tjs_char *method, const tjs_char *url, const tjs_char *userName, const tjs_char *password) {
		abort();
		if (http.open(method, url, userName, password)) {
			onReadyStateChange(READYSTATE_OPEN);
		} else {
			TVPThrowExceptionMessage(http.getErrorMessage());
		}
	}

	static tjs_error open(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, HttpRequest *self) {
		if (numparams < 2) {
			return TJS_E_BADPARAMCOUNT;
		}
		self->_open(params[0]->GetString(), params[1]->GetString(), numparams > 2 ? params[2]->GetString() : NULL, numparams > 3 ? params[3]->GetString() : NULL);
		return TJS_S_OK;
	}
	
	/**
	 * 送信時に送られるヘッダーを追加する
	 * @param name ヘッダ名
	 * @param value 値
	 */
	void setRequestHeader(const tjs_char *name, const tjs_char *value) {
		checkRunning();
		http.addHeader(name, value);
	}

	/**
	 * リクエストの送信
	 * @param data 送信するデータ
	 * 文字列の場合：そのまま送信
	 * 辞書の場合: application/x-www-form-urlencoded で送信
	 */
	void _send(const void *data, int size) {
		checkRunning();
		checkOpen();
		if (data) {
		}
		startThread();
	}

	static tjs_error send(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, HttpRequest *self) {
		void *data = NULL;
		int size = 0;
		if (numparams > 0) {
			switch (params[0]->Type()) {
			case tvtVoid:
				break;
			case tvtObject:
				break;
			case tvtString:
				// UTF-8 に変換
				break;
			case tvtOctet:
				break;
			default:
				break;
			}
		}
		self->_send(data, size);
		return TJS_S_OK;
	}

	/**
	 * 現在実行中の送受信のキャンセル
	 */
	void abort() {
		stopThread();
	}
	
	/**
	 * すべての HTTPヘッダを取得する
	 * @return HTTPヘッダが格納された辞書
	 */
	tTJSVariant getAllResponseHeaders() {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		tstring name;
		tstring value;
		http.initRH();
		while (http.getNextRH(name, value)) {
			tTJSVariant v(value.c_str());
			dict->PropSet(TJS_MEMBERENSURE, name.c_str(), NULL, &v, dict);
		}
		return tTJSVariant(dict,dict);
	}

	/**
	 * 指定したHTTPヘッダを取得する
	 * @param name ヘッダラベル名
	 * @return ヘッダの値
	 */
	const tjs_char *getResponseHeader(const tjs_char *name) {
		return http.getResponseHeader(name);
	}

	/**
	 * 通信状態。読み込み専用
	 * @return 現在の通信状態
	 * 0: 初期状態
	 * 1: 読み込み中
	 * 2: 読み込んだ
	 * 3: 解析中
	 * 4: 完了
	 */
	int getReadyState() const {
		return readyState;
	}

	/**
	 * レスポンスデータ。読み込み専用
	 * @return レスポンスデータ
	 */
	tTJSVariant getResponse() {
		return tTJSVariant();
	}
		
	/**
	 * レスポンスの HTTPステータスコード。読み込み専用
	 * @return ステータスコード
	 */
	int getStatus() {
		return statusCode;
	}
	
	/**
	 * レスポンスの HTTPステータスの文字列
	 * @return レスポンス文字列
	 */
	const tjs_char *getStatusText() {
		return statusText.c_str();
	}

	/**
	 * インスタンス生成ファクトリ
	 */
	static tjs_error factory(HttpRequest **result, tjs_int numparams, tTJSVariant **params, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		iTJSDispatch2 *window = params[0]->AsObjectNoAddRef();
		if (window->IsInstanceOf(0, NULL, NULL, L"Window", window) != TJS_S_TRUE) {
			TVPThrowExceptionMessage(L"InvalidObject");
		}
		*result = new HttpRequest(objthis, window, numparams >= 2 ? params[1]->AsInteger() != 0 : true, AGENT_NAME);
		return S_OK;
	}
	

protected:

	void checkRunning() {
		if (threadHandle) {
			TVPThrowExceptionMessage(TJS_W("already running"));
		}
	}

	void checkOpen() {
		if (!http.isValid()) {
			TVPThrowExceptionMessage(TJS_W("not open"));
		}
	}

	/**
	 * readyState が変化した場合のイベント処理
	 * @param readyState 新しいステート
	 * @return 中断する場合は true を返す
	 */
	void onReadyStateChange(int readyState) {
		this->readyState = readyState;
		if (readyState == READYSTATE_LOADED) {
			stopThread();
		}
		tTJSVariant param(readyState);
		static ttstr eventName(TJS_W("onReadyStateChange"));
		TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_POST, 1, &param);
	}
	
	/**
	 * データ読み込み中のイベント処理
	 * @param percent 進捗
	 * @return 中断する場合は true を返す
	 */
	void onProgress(int percent) {
		tTJSVariant param(percent);
		static ttstr eventName(TJS_W("onProgress"));
		TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_POST, 1, &param);
	}

	
	// ユーザメッセージレシーバの登録/解除
	void setReceiver(bool enable) {
		tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
		tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
		tTJSVariant userdata = (tTVInteger)(tjs_int)this;
		tTJSVariant *p[] = {&mode, &proc, &userdata};
		if (window->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, objthis) != TJS_S_OK) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}

	/**
	 * イベント受信処理
	 */
	static bool __stdcall receiver(void *userdata, tTVPWindowMessage *Message) {
		HttpRequest *self = (HttpRequest*)userdata;
		switch (Message->Msg) {
		case WM_HTTP_READYSTATE:
			if (self == (HttpRequest*)Message->WParam) {
				self->onReadyStateChange((ReadyState)Message->LParam);
				return true;
			}
			break;
		case WM_HTTP_PROGRESS:
			if (self == (HttpRequest*)Message->WParam) {
				self->onProgress((int)Message->LParam);
				return true;
			}
			break;
		}
		return false;
	}

	// -----------------------------------------------
	// スレッド処理
	// -----------------------------------------------

	// メッセージ送信
	void postMessage(UINT msg, WPARAM wparam=NULL, LPARAM lparam=NULL) {
		// ウィンドウハンドルを取得して通知
		tTJSVariant val;
		window->PropGet(0, TJS_W("HWND"), NULL, &val, objthis);
		HWND hwnd = reinterpret_cast<HWND>((tjs_int)(val));
		::PostMessage(hwnd, msg, wparam, lparam);
	}

	bool download(void *buffer, DWORD size, int percent) {
		if (buffer) {
			if (output) {
				output->Write(buffer, size, &size);
			}
		} else {
			if (output) {
				output->Release();
			}
		}
		postMessage(WM_HTTP_PROGRESS, (WPARAM)this, percent);
		return !canceled;
	}
	
	/**
	 * 通信時のコールバック処理
	 * @return キャンセルなら false
	 */
	static bool downloadCallback(void *context, void *buffer, DWORD size, int percent) {
		HttpRequest *self = (HttpRequest*)context;
		return self ? self->download(buffer, size, percent) : false;
	}
	
	void threadMain() {
		postMessage(WM_HTTP_READYSTATE, (WPARAM)this, (LPARAM)READYSTATE_SENT);
		if (!http.request()) {
			statusCode = 0;
			statusText = http.getErrorMessage();
		} else {
			postMessage(WM_HTTP_READYSTATE, (WPARAM)this, (LPARAM)READYSTATE_RECEIVING);
			//IStream *out = TVPCreateIStream(ttstr(saveFileName), TJS_BS_WRITE);
			

			switch (http.response(downloadCallback, (void*)this)) {
			case HttpConnection::ERROR_NONE:
				statusCode = http.getStatusCode();
				statusText = http.getStatusText();
				break;
			case HttpConnection::ERROR_CANCEL:
				statusCode = -1;
				statusText = L"aborted";
				break;
			default:
				statusCode = 0;
				statusText = http.getErrorMessage();
				break;
			}
		}
		postMessage(WM_HTTP_READYSTATE, (WPARAM)this, (LPARAM)READYSTATE_LOADED);
	}

	// 実行スレッド
	static unsigned __stdcall threadFunc(void *data) {
		((HttpRequest*)data)->threadMain();
		_endthreadex(0);
		return 0;
	}

	// スレッド処理開始
	void startThread() {
		stopThread();
		canceled = false;
		threadHandle = (HANDLE)_beginthreadex(NULL, 0, threadFunc, this, 0, NULL);
	}

	// スレッド処理終了
	void stopThread() {
		if (threadHandle) {
			canceled = true;
			WaitForSingleObject(threadHandle, INFINITE);
			CloseHandle(threadHandle);
			threadHandle = 0;
		}
	}
	
private:
	iTJSDispatch2 *objthis; ///< オブジェクト情報の参照
	iTJSDispatch2 *window; ///< オブジェクト情報の参照

	// 通信用処理
	HttpConnection http;

	// 処理用
	HANDLE threadHandle; ///< スレッドのハンドル
	bool canceled; ///< キャンセルされた

	// 結果
	IStream *output;
	int readyState;
	int statusCode;
	ttstr statusText;
};

#define ENUM(n) Variant(#n, (int)HttpRequest::READYSTATE_ ## n)

NCB_REGISTER_CLASS(HttpRequest) {
	Factory(&ClassT::factory);
	ENUM(UNINITIALIZED);
	ENUM(OPEN);
	ENUM(SENT);
	ENUM(RECEIVING);
	ENUM(LOADED);
	RawCallback(TJS_W("open"), &Class::open, 0);
	NCB_METHOD(setRequestHeader);
	RawCallback(TJS_W("send"), &Class::send, 0);
	NCB_METHOD(abort);
	NCB_METHOD(getAllResponseHeaders);
	NCB_METHOD(getResponseHeader);
	NCB_PROPERTY_RO(readyState, getReadyState);
	NCB_PROPERTY_RO(response, getResponse);
	NCB_PROPERTY_RO(status, getStatus);
	NCB_PROPERTY_RO(statusText, getStatusText);
}

static void
PreRegistCallback()
{
	// Content-Type 取得用の正規表現オブジェクトを取得
	regctype = new tTJSVariant();
	TVPExecuteExpression(TJS_W("new RegExp(\"<meta[ \\t]+http-equiv=(\\\"content-type\\\"|'content-type'|content-type)[ \\t]+content=(\\\"[^\\\"]*\\\"|'[^']*'|[^ \\t>]+).*>\",\"i\")"), regctype);
}

static void
PostUnregistCallback()
{
	// Content-Type 取得用の正規表現オブジェクトを解放
	delete regctype;
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
