#include "ncbind.hpp"
#include "serv.hpp"

// メッセージコード
#define	WM_HTTP_REQUEST	(WM_APP+8)	// リクエストされた

class SimpleHTTPServerResponse
{
public:
	typedef SimpleHTTPServerResponse SelfClass;
	enum TransferType { UNKNOWN, TEXT, FILE, BINARY };
	typedef PwRequestResponse::String PwStr;
	typedef PwRequestResponse::Size   PwSize;
private:
	iTJSDispatch2 *owner;
	PwRequestResponse *rr;

	int   status, transfer_type, codepage;
	PwSize           transfer_binsize;
	tjs_uint8 const *transfer_binptr;
	tjs_uint8       *transfer_binptralloced;
	ttstr content_type, transfer_text, transfer_file;

	ncbDictionaryAccessor arg;
	tTJSVariant result;

	PwStr toNarrowString(ttstr const &ref) {
		PwStr ret;
		tjs_int len = ref.GetNarrowStrLen();
		tjs_nchar *tmp = new tjs_nchar[len+1];
		ref.ToNarrowStr(tmp, len+1);
		ret.assign(  tmp, len);
		delete [] tmp;
		return ret;
	}

	static void TJS_USERENTRY TryRequestBlock(void *data) {
		SelfClass *self = (SelfClass*)data;
		if (self) self->callOnRequest();
	}
	static bool TJS_USERENTRY CatchRequestBlock(void *data, const tTVPExceptionDesc & desc) {
		SelfClass *self = (SelfClass*)data;
		if (self) self->onRequestError(desc);
		return false;
	}

	static void NameValueCallback(const PwStr &key, const PwStr &value, void *data) {
		ncbPropAccessor *dic = (ncbPropAccessor*)data;
		if (dic != NULL && dic->IsValid()) {
			ttstr vkey(key.c_str()), vval(value.c_str());
			dic->SetValue(vkey.c_str(), vval);
		}
	}

public:
	~SimpleHTTPServerResponse() {
		if (transfer_binptralloced) delete [] transfer_binptralloced;
	}
	SimpleHTTPServerResponse(iTJSDispatch2 *obj, PwRequestResponse *_rr) :
	owner(obj), rr(_rr), status(0), transfer_type(UNKNOWN), codepage(CP_UTF8),
	transfer_binsize(0), transfer_binptr(0), transfer_binptralloced(0) {
		ncbDictionaryAccessor head;
		ncbDictionaryAccessor form;

		rr->getHeader  (NameValueCallback, (void*)&head);
		rr->getFormData(NameValueCallback, (void*)&form);

		arg.SetValue(TJS_W("method"),  rr->getMethod());
		arg.SetValue(TJS_W("request"), rr->getURI());
		arg.SetValue(TJS_W("host"),    rr->getHost());
		arg.SetValue(TJS_W("client"),  rr->getClient());
		arg.SetValue(TJS_W("header"),  tTJSVariant(head, head));
		arg.SetValue(TJS_W("form"),    tTJSVariant(form, form));
	}

	void request() {
		TVPDoTryBlock(TryRequestBlock, CatchRequestBlock, NULL, (void*)this);
	}
	void response() {
		PwStr tempstr;
		// ファイル転送時は先に存在チェック（無ければ404）
		if (transfer_type == FILE) {
			ttstr fname = TVPGetPlacedPath(transfer_file);
			if (fname.length() > 0 && TVPIsExistentStorage(fname)) {
				if (fname.length() > 0 && !wcschr(fname.c_str(), '>')) {
					// アーカイブ内の場合はオンメモリでバイナリ転送に変更する
					IStream *file = TVPCreateIStream(fname, TJS_BS_READ);
					if (!file) setError(404, TJS_W("file not found"), transfer_file.c_str());
					else {
						transfer_binptralloced = 0;
						try {
							STATSTG stat;
							file->Stat(&stat, STATFLAG_NONAME);
							PwSize size = (PwSize)stat.cbSize.QuadPart; // XXX 巨大ファイルは無理
							tjs_uint8 *p = new tjs_uint8[size];
							file->Read(p, size, &transfer_binsize);
							transfer_binptr = transfer_binptralloced = p;
							transfer_type = BINARY;
						} catch (...) {
							if (file) file->Release();
							if (transfer_binptralloced) delete [] transfer_binptralloced;
							transfer_binptralloced = 0;
							throw;
						}
						if (file) file->Release();
					}
				} else {
					// 通常のファイルはローカルパスで直接渡す
					TVPGetLocalName(fname);
					transfer_file = fname;
				}
			} else {
				setError(404, TJS_W("file not found"), transfer_file.c_str());
			}
		}
		// ステータス設定
		if (status > 0) {
			PwStr tmp(toNarrowString(ttstr((tjs_int)status)));
			rr->setStatus(tmp.c_str());
		}

		// content-type設定
		if (content_type.length() > 0) {
			PwStr tmp(toNarrowString(content_type));
			rr->setContentType(tmp.c_str());
			tmp = rr->getCharset(tmp.c_str());
			ttstr cset(tmp.c_str());
			if (cset.length() > 0) {
				cset.ToLowerCase();
				// 投げやりな charset -> codepage 判定
				if      (cset.StartsWith(TJS_W("utf-8"      ))) codepage = CP_UTF8;
				else if (cset.StartsWith(TJS_W("shift"      ))) codepage = 932;
				else if (cset.StartsWith(TJS_W("sjis"       ))) codepage = 932;
				else if (cset.StartsWith(TJS_W("x-sjis"     ))) codepage = 932;
				else if (cset.StartsWith(TJS_W("windows-31j"))) codepage = 932;
				else if (cset.StartsWith(TJS_W("euc"        ))) codepage = 51932;
				else if (cset.StartsWith(TJS_W("iso-2022-jp"))) codepage = 50220;
				else if (cset.StartsWith(TJS_W("jis"        ))) codepage = 50220;
			}
		}
		// 転送処理
		switch (transfer_type) {
		case UNKNOWN:
			rr->sendBuffer("", 0);
		case FILE:
			{
				PwStr tmp(toNarrowString(transfer_file));
				rr->sendFile(tmp.c_str());
			}
			break;
		case TEXT:
			{
				int rlen = transfer_text.length();
				transfer_binsize = ::WideCharToMultiByte(codepage, 0, transfer_text.c_str(), rlen, NULL, 0, NULL, NULL);
				transfer_binptralloced  = new tjs_uint8[transfer_binsize + 1];
				transfer_binsize = ::WideCharToMultiByte(codepage, 0, transfer_text.c_str(), rlen, (LPSTR)transfer_binptralloced, transfer_binsize, NULL, NULL);
				transfer_binptralloced[transfer_binsize] = 0;
				transfer_binptr = transfer_binptralloced;
			}
			/* NOT BREAK */
		case BINARY:
			rr->sendBuffer(transfer_binptr, transfer_binsize);
			break;
		}
	}
	void callOnRequest() {
		tTJSVariantClosure closure(owner, owner);
		tTJSVariant param(arg, arg), *p[] = { &param };
		const tjs_char *method = TJS_W("onRequest");
		if (TJS_SUCCEEDED(closure.FuncCall(0, method, NULL, &result, 1, p, NULL))) {
			if (result.Type() == tvtObject) setResult(result.AsObjectNoAddRef(),   method);
			else
				setError(500, TJS_W("response"), TJS_W("no object returned from"), method);
		} else  setError(500, TJS_W("callback"), TJS_W("failed to call"),          method);
	}
	void onRequestError(const tTVPExceptionDesc & desc) {
		setError(500, desc.type.c_str(), desc.message.c_str());
	}

	// textデータ
	void setTransferText(ttstr const& text) {
		transfer_type = TEXT;
		transfer_text = text;
		if (status < 0) status = 200;
	}
	// file(IStream)データ
	void setTransferFile(ttstr const& file) {
		transfer_type = FILE;
		transfer_file = file;
		if (status < 0) status = 200;
	}
	/*
	// Layerデータ
	void setTransferLayer(tTJSVariant var, const tjs_char* method) {
		// まだ未実装
		setError(501, TJS_W("not implemented"), TJS_W("layer transfer returned from"), method);
	}
	 */
	// Arrayデータ
	void setTransferObject(iTJSDispatch2 *obj, const tjs_char* method) {
		if (!obj->IsInstanceOf(0, NULL, NULL, TJS_W("Array"), obj)) 
			setError(501, TJS_W("invalid data"), TJS_W("no array object returned from"), method);
		else {
			ncbPropAccessor arr(obj);
			transfer_binsize = arr.GetArrayCount();
			tjs_uint8 *p  = new tjs_uint8[transfer_binsize];
			transfer_binptr = transfer_binptralloced = p;
			for (PwSize i = 0; i < transfer_binsize; i++) {
				p[i] = arr.getIntValue(i) & 0xFF;
			}
		}
	}
	// Octetデータ
	void setTransferOctet(tTJSVariantOctet *oct, const tjs_char* method) {
		transfer_binsize = oct->GetLength();
		if (!transfer_binsize) 
			setError(500, TJS_W("no data"), TJS_W("zero-length octet transfer returned from"), method);
		transfer_binptr = oct->GetData();
	}

	// レスポンスデータを解析
	void setResult(iTJSDispatch2 *obj, const tjs_char* method) {
		ncbPropAccessor dic(obj);
		status         = dic.getIntValue(TJS_W("status"), -1);
		content_type   = dic.getStrValue(TJS_W("content_type"));

		// 転送タイプ別処理
		ncbTypedefs::Tag<tTJSVariant> tag;
		if      (dic.HasValue(TJS_W("text")))  setTransferText (dic.getStrValue(TJS_W("text")));
		else if (dic.HasValue(TJS_W("file")))  setTransferFile (dic.getStrValue(TJS_W("file")));
//		else if (dic.HasValue(TJS_W("layer"))) setTransferLayer(dic.GetValue(TJS_W("layer"), tag), method);
		else if (dic.HasValue(TJS_W("binary"))) {
			tTJSVariant v(dic.GetValue(TJS_W("binary"), tag));
			switch (v.Type()) {
			case tvtObject: setTransferObject(v.AsObjectNoAddRef(), method); break;
			case tvtOctet:  setTransferOctet (v.AsOctetNoAddRef(),  method); break;
			default:
				setError(500, TJS_W("invalid data"), TJS_W("invalid binary data returned from"), method);
				break;
			}
		}

		// リダイレクト処理
		ttstr redirect = dic.getStrValue(TJS_W("redirect"));
		if (redirect.length() > 0) {
			if (status != 301 && status != 302 && status != 307) status = 301;
			PwStr tmp(toNarrowString(redirect));
			rr->setRedirect(tmp.c_str());
			if (transfer_type == UNKNOWN) {
				content_type  =  TJS_W("text/html");
				ttstr refresh =  TJS_W("<html><head><meta http-equiv=refresh content=\"0; url=");
				refresh += redirect;
				refresh += TJS_W("\"></head><body><a href=\"");
				refresh += redirect;
				refresh += TJS_W("\">Redirect...</a></body></html>");
				setTransferText(refresh);
			}
		}

		// 何も転送するものが無い
		if (transfer_type == UNKNOWN || status < 0)
			setError(500, TJS_W("no data"), TJS_W("no transfer data returned from"), method);
	}

	void setError(int st, ttstr type, ttstr desc) {
		status = st;
		ttstr numstr((tjs_int)status);
		PwStr reason(toNarrowString(numstr));
		reason = rr->getReason(reason.c_str());
		content_type = "text/html; charset=utf-8";
		ttstr error(TJS_W("<html><head><title>Error</title></head><body><h1>"));
		error += numstr;
		error += TJS_W(" ");
		error += ttstr(reason.c_str());
		error += TJS_W("</h1><p>");
		error += type;
		error += TJS_W(" : ");
		error += desc;
		error += TJS_W("</p></body></html>");
		setTransferText(error);
	}
	void setError(int st, const tjs_char *type, const tjs_char *desc, const tjs_char *method) {
		ttstr t(desc);
		t += TJS_W(" ");
		t += method;
		t += TJS_W("method.");
		setError(st, ttstr(type), t);
	}
};

class SimpleHTTPServer
{
public:
	typedef SimpleHTTPServer SelfClass;
private:
	iTJSDispatch2 *self;
	PwHTTPServer *instance;
	HWND message;
	int port, timeout;

	static ATOM WindowClass;
	void createMessageWindow() {
		HINSTANCE hinst = ::GetModuleHandle(NULL);
		if (!WindowClass) {
			WNDCLASSEXW wcex = {
				/*size*/sizeof(WNDCLASSEX), /*style*/0, /*proc*/WndProc, /*extra*/0L,0L, /*hinst*/hinst,
				/*icon*/NULL, /*cursor*/NULL, /*brush*/NULL, /*menu*/NULL,
				/*class*/TJS_W("SimpleHTTPServer Message Window Class"), /*smicon*/NULL };
			WindowClass = ::RegisterClassExW(&wcex);
			if (!WindowClass)
				TVPThrowExceptionMessage(TJS_W("register window class failed."));
		}
		if (!message) {
			message = ::CreateWindowExW(0, (LPCWSTR)MAKELONG(WindowClass, 0), TJS_W("SimpleHHTPServer Message"),
										0, 0, 0, 1, 1, HWND_MESSAGE, NULL, hinst, NULL);
			if (!message)
				TVPThrowExceptionMessage(TJS_W("create message window failed."));
			::SetWindowLong(message, GWL_USERDATA, (LONG)this);
		}
	}
	static LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		if (msg == WM_HTTP_REQUEST) {
			SelfClass *self = (SelfClass*)(::GetWindowLong(hwnd, GWL_USERDATA));
			PwRequestResponse *rr = (PwRequestResponse*)lp;
			if (self && rr) self->onRequest(rr);
			if (rr) rr->done();
			return 0;
		}
		return DefWindowProc(hwnd, msg, wp, lp);
	}

public:
	SimpleHTTPServer(iTJSDispatch2 *obj, tjs_int n, tTJSVariant **p) : self(obj), instance(0), message(0), port(0), timeout(10) {
		if (n > 0 && p[0]->Type() == tvtInteger) port    = (int)p[0]->AsInteger();
		if (n > 1 && p[0]->Type() == tvtInteger) timeout = (int)p[1]->AsInteger();
		instance = PwHTTPServer::Factory(&RequestCallback, (void*)this, timeout);
		createMessageWindow();
	}
	~SimpleHTTPServer() {
		if (message) ::DestroyWindow(message);
		message = NULL;
		stop();
		if (instance) delete instance;
	}
	static tjs_error TJS_INTF_METHOD Factory(SimpleHTTPServer **instance, tjs_int n, tTJSVariant **p, iTJSDispatch2 *self) {
		*instance = new SimpleHTTPServer(self, n, p);
		return TJS_S_OK;
	}

	// 別スレッドから呼ばれるのでメッセージウィンドウにメッセージを投げてメインスレッド側で実行する
	static void RequestCallback(PwRequestResponse *rr, void *param, int reason) {
		SelfClass *self = (SelfClass*)param;
		if (!self) return;
		if (reason) {
			HWND hwnd = self->message; // 読み込みのみなのでロックは不要…だと思う
			if (hwnd) ::PostMessage(hwnd, WM_HTTP_REQUEST, 0, (LPARAM)rr);
		} else {
			// タイムアウト時処理（キャンセルをかける？）
			// 現状では何もしない
		}
	}

	// メッセージウィンドウからの呼び返しによってリクエストに対応
	void onRequest(PwRequestResponse *rr) {
		SimpleHTTPServerResponse res(self, rr);
		res.request();
		res.response();
	}

	int  getPort()    const { return port; }
	int  getTimeout() const { return timeout; }
	int  start() { if (instance) port = instance->start(port); return port; }
	void stop()  { if (instance)        instance->stop(); }

};

ATOM SimpleHTTPServer::WindowClass = 0;


NCB_REGISTER_CLASS(SimpleHTTPServer)
{
	Factory(&Class::Factory);
	Property(TJS_W("port"   ), &Class::getPort,    0);
	Property(TJS_W("timeout"), &Class::getTimeout, 0);
	Method(  TJS_W("start"), &Class::start);
	Method(  TJS_W("stop" ), &Class::stop);
}

