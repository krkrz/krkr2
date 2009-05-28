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
		   output(NULL), outputLength(0), input(NULL), inputLength(0),
		   readyState(READYSTATE_UNINITIALIZED), statusCode(0)
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
	static tjs_error send(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, HttpRequest *self) {
		self->checkRunning();
		self->checkOpen();
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
		//IStream *out = TVPCreateIStream(ttstr(saveFileName), TJS_BS_WRITE);
		self->startThread();
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
	 * @param upload 送信中
	 * @param percent 進捗
	 */
	void onProgress(bool upload, int percent) {
		tTJSVariant params[2];
		params[0] = upload;
		params[1] = percent;
		static ttstr eventName(TJS_W("onProgress"));
		TVPPostEvent(objthis, objthis, eventName, 0, TVP_EPT_POST, 2, params);
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
				int lparam = (int)Message->LParam;
				self->onProgress((lparam & 0xff00)!=0, (lparam & 0xff));
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

	/**
	 * ファイル送信処理
	 * @param buffer 読み取りバッファ
	 * @param size 読み出したサイズ
	 */
	bool upload(void *buffer, DWORD &size) {
		if (input) {
			input->Read(buffer, size, &size);
		} else {
			size = 0;
		}
		inputSize += size;
		int percent = (inputLength > 0) ? inputSize * 100 / inputLength : 0;
		postMessage(WM_HTTP_PROGRESS, (WPARAM)this, 0x0100 | percent);
		return !canceled;
	}

	/**
	 * 通信時のコールバック処理
	 * @return キャンセルなら false
	 */
	static bool uploadCallback(void *context, void *buffer, DWORD &size) {
		HttpRequest *self = (HttpRequest*)context;
		return self ? self->upload(buffer, size) : false;
	}
	
	/**
	 * ファイル読み取り処理
	 * @param buffer 読み取りバッファ
	 * @param size 読み出したサイズ
	 */
	bool download(const void *buffer, DWORD size) {
		if (output) {
			if (buffer) {
				output->Write(buffer, size, &size);
			} else {
				output->Release();
			}
		}
		outputSize += size;
		int percent = (outputLength > 0) ? outputSize * 100 / outputLength : 0;
		postMessage(WM_HTTP_PROGRESS, (WPARAM)this, percent);
		return !canceled;
	}
	
	/**
	 * 通信時のコールバック処理
	 * @return キャンセルなら false
	 */
	static bool downloadCallback(void *context, const void *buffer, DWORD size) {
		HttpRequest *self = (HttpRequest*)context;
		return self ? self->download(buffer, size) : false;
	}

	/**
	 * バックグラウンドで実行する処理
	 */
	void threadMain() {
		postMessage(WM_HTTP_READYSTATE, (WPARAM)this, (LPARAM)READYSTATE_SENT);
		inputSize = 0;
		int errorCode;
		if ((errorCode = http.request(uploadCallback, (void*)this)) == HttpConnection::ERROR_NONE) {
			http.queryInfo();
			outputSize = 0;
			outputLength = http.getContentLength();
			postMessage(WM_HTTP_READYSTATE, (WPARAM)this, (LPARAM)READYSTATE_RECEIVING);
			errorCode = http.response(downloadCallback, (void*)this);
		}
		switch (errorCode) {
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

	// リクエスト
	IStream *input;
	int inputLength;
	int inputSize;

	// レスポンス
	IStream *output;
	int outputLength;
	int outputSize;

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
