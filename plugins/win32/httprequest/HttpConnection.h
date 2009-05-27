#ifndef __HTTPCONNECTION_H_
#define __HTTPCONNECTION_H_

#include <windows.h>
#include <tchar.h>
#include <WinInet.h>
#include <vector>
#include <string>
#include <map>

#include "Base64.h"

using namespace std;
typedef basic_string<TCHAR> tstring;

/**
 * HTTP接続を実現するクラス
 */
class HttpConnection
{

public:
	// エラー状態
	enum Error {
		ERROR_NONE,  // エラーなし
		ERROR_INET,  // ネットワークライブラリのエラー
		ERROR_FILE,  // ファイルアクセスエラー
		ERROR_CANCEL // キャンセルされた
	};

	/**
	 * コールバック処理
	 * @param context コンテキスト
	 * @param buffer データバッファ。最後は NULL
	 * @param size データバッファのサイズ。最後は0
	 * @param percent データ状況。最後は0
	 * @return 中断する場合は true を返す
	 */
	typedef bool (*DownloadCallback)(void *context, void *buffer, DWORD size, int percent);
	
	/**
	 * コンストラクタ
	 * @param agentName エージェント名
	 * @param checkCert 認証確認するかどうか
	 */
	HttpConnection(tstring agentName, bool checkCert=false) : agentName(agentName), checkCert(checkCert), contentLength(0), secure(false){
		hInet = NULL;
		hConn = NULL;
		hReq  = NULL;
	}

	// デストラクタ
	~HttpConnection(void) {
		clearParam();
	}

	// 送信ヘッダをクリア
	void clearHeader() {
		header.clear();
		requestContentType.resize(0);
		requestEncoding.resize(0);
	}

	// ハンドルをクリア
	void closeHandle() {
		if (hReq) { InternetCloseHandle(hReq);hReq=NULL; }
		if (hConn) { InternetCloseHandle(hConn);hReq=NULL; }
		if (hInet) { InternetCloseHandle(hInet);hReq=NULL; }
	}

	// 送信パラメータをクリア(名前を変えただけで、実体は送信データクリア)
	void clearParam() {
		closeHandle();
		clearHeader();
	}

	// ----------------------------------------------------------------------------------------
	
	// HTTP ヘッダを追加する
	void addHeader(const TCHAR *name, const TCHAR *value);

	// 認証ヘッダをセットする(addHeader のユーティリティ)
	void addBasicAuthHeader(const tstring &user, const tstring &passwd) {
		tstring sendStr = user + _T(":") + passwd;
		tstring value = _T("Basic") + base64encode(sendStr.c_str(), sendStr.length());
		addHeader(_T("Authorization"), value.c_str());
	}
	
	// ----------------------------------------------------------------------------------------------------
	
	/**
	 * リクエスト開始
	 * @param method アクセスメソッド
	 * @param url URL
	 * @param user アクセスユーザ
	 * @param passwd アクセスパスワード
	 */
	bool open(const TCHAR *method,
			  const TCHAR *url,
			  const TCHAR *user = NULL,
			  const TCHAR *passwd = NULL);

	/**
	 * リクエスト送信
	 * @param data 送信データ
	 * @param data 送信データサイズ
	 */
	bool request(const BYTE *data=NULL, int dataSize=0);

	/**
	 * 受信処理
	 * @param callback 保存用コールバック
	 * @param context コールバック用コンテキスト
	 */
	int response(DownloadCallback callback=NULL, void *context=NULL);
	
	// ----------------------------------------------------------------------------------------------------
	
	// エラーメッセージの取得
	const TCHAR *getErrorMessage() const {
		return errorMessage.c_str();
	}
	
	// 最後のリクエストが成功しているかどうか
	bool isValid() const {
		return hReq != NULL;
	}

	// HTTPステータスコードを取得
	int getStatusCode() const {
		return statusCode;
	}

	// HTTPステータステキストを取得
	const TCHAR *getStatusText() const {
		return statusText.c_str();
	}
	
	// 取得されたコンテンツの長さ
	int getContentLength() const {
		return contentLength;
	}

	// コンテンツの MIME-TYPE
	const TCHAR *getContentType() const {
		return contentType.c_str();
	}

	// コンテンツのエンコーディング情報
	const TCHAR *getEncoding() const {
		return encoding.c_str();
	}
	
	/**
	 * レスポンスのヘッダ情報を取得
	 */
	const TCHAR *getResponseHeader(const TCHAR *name) {
		map<tstring,tstring>::const_iterator it = responseHeaders.find(tstring(name));
		if (it != responseHeaders.end()) {
			return it->second.c_str();
		}
		return NULL;
	}

	// レスポンスヘッダ全取得用:初期化
	void initRH() {
		rhit = responseHeaders.begin();
	}

	// レスポンスヘッダ全取得用:取得
	bool getNextRH(tstring &name, tstring &value) {
		if (rhit != responseHeaders.end()) {
			name  = rhit->first;
			value = rhit->second;
			rhit++;
			return true;
		}
		return false;
	}

	bool getCheckCert() const { return checkCert;	}
	void setCheckCert(bool check)	{ checkCert = check;}

private:
	// 基礎情報
	tstring agentName; ///< ユーザエージェント名
	bool checkCert;	   ///< 証明書確認ダイアログを出すか
	bool secure;       ///< https 通信かどうか

	HINTERNET hInet; ///< インターネット接続
	HINTERNET hConn; ///< コネクション
	HINTERNET hReq;  ///< HTTPリクエスト
	
	// 送信用データ
	vector<tstring> header;	///< HTTP ヘッダ
	tstring requestContentType; ///< リクエストの Content-Type:
	tstring requestEncoding;    ///< リクエストのエンコード指定

	// 受信用データ
	DWORD contentLength;     ///< Content-Length:
	tstring contentType;     ///< Content-Type: のtype部
	tstring encoding;        ///< Content-TYpe: のエンコーディング部
	DWORD statusCode;        ///< HTTP status code
	tstring statusText;      ///< HTTP status text
	map<tstring,tstring> responseHeaders; ///< レスポンスヘッダ
	map<tstring,tstring>::const_iterator rhit; //< レスポンスヘッダ参照用イテレータ

	// エラーコード
	tstring errorMessage; ///< エラーメッセージ
};

#endif
