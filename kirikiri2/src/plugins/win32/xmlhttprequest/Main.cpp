/*
 * XMLHttpRequest
 *
 * http://www.w3.org/TR/XMLHttpRequest/
 *
 * Written by Kouhei Yanagita
 *
 */
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include "tp_stub.h"
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#pragma comment(lib, "WSock32.lib")
//---------------------------------------------------------------------------



/*
    プラグイン側でネイティブ実装されたクラスを提供し、吉里吉里側で使用できるように
    する例です。

    ネイティブクラスは tTJSNativeInstance を継承したクラス上に作成し、そのネイ
    ティブクラスとのインターフェースを tTJSNativeClassForPlugin をベースに作成し
    ます。

    「TJS2 リファレンス」の「組み込みの手引き」の「基本的な使い方」にある例と同じ
    クラスをここでは作成します。ただし、プラグインで実装する都合上、TJS2 リファ
    レンスにある例とは若干実装の仕方が異なることに注意してください。
*/



//---------------------------------------------------------------------------
// テストクラス
//---------------------------------------------------------------------------
/*
    各オブジェクト (iTJSDispatch2 インターフェース) にはネイティブインスタンスと
    呼ばれる、iTJSNativeInstance 型のオブジェクトを登録することができ、これを
    オブジェクトから取り出すことができます。
    まず、ネイティブインスタンスの実装です。ネイティブインスタンスを実装するには
    tTJSNativeInstance からクラスを導出します。tTJSNativeInstance は
    iTJSNativeInstance の基本的な動作を実装しています。
*/
class NI_XMLHttpRequest : public tTJSNativeInstance // ネイティブインスタンス
{
public:
    NI_XMLHttpRequest()
    {

    }

    tjs_error TJS_INTF_METHOD
        Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
    {
        // TJS2 オブジェクトが作成されるときに呼ばれる

        Initialize();
        _target = tjs_obj;

        if (++objcount == 1) {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 0), &wsaData)) {
                // error
            }
        }

        _hThread = NULL;
        InitializeCriticalSection(&_criticalSection);

        return S_OK;
    }

    void TJS_INTF_METHOD Invalidate()
    {
        // オブジェクトが無効化されるときに呼ばれる

        if (_hThread) {
            CloseHandle(_hThread);
            _hThread = NULL;
        }

        if (--objcount == 0) {
            WSACleanup();
        }

        DeleteCriticalSection(&_criticalSection);
    }


    void Initialize(void) {
        _responseData.clear();
        _responseBody.clear();
        _responseStatus = 0;
        _requestHeaders.clear();
        _aborted = false;
    }

    tjs_int GetReadyState(void) const { return _readyState; }
    void SetReadyState(tjs_int v) { _readyState = v; OnReadyStateChange(); }

    tjs_int GetResponseStatus(void) const {
        RaiseExceptionIfNotResponsed();
        return _responseStatus;
    }

    ttstr GetResponseStatusText(void) const {
        RaiseExceptionIfNotResponsed();

        switch (_responseStatus) {
        case 200: return ttstr("OK");
        case 201: return ttstr("Created");
        case 202: return ttstr("Accepted");
        case 203: return ttstr("Non-Authoritative Information");
        case 204: return ttstr("No Content");
        case 205: return ttstr("Reset Content");
        case 206: return ttstr("Partial Content");
        case 300: return ttstr("Multiple Choices");
        case 301: return ttstr("Moved Permanently");
        case 302: return ttstr("Found");
        case 303: return ttstr("See Other");
        case 304: return ttstr("Not Modified");
        case 305: return ttstr("Use Proxy");
        case 307: return ttstr("Temporary Redirect");
        case 400: return ttstr("Bad Request");
        case 401: return ttstr("Unauthorized");
        case 402: return ttstr("Payment Required");
        case 403: return ttstr("Forbidden");
        case 404: return ttstr("Not Found");
        case 405: return ttstr("Method Not Allowed");
        case 406: return ttstr("Not Acceptable");
        case 407: return ttstr("Proxy Authentication Required");
        case 408: return ttstr("Request Timeout");
        case 409: return ttstr("Conflict");
        case 410: return ttstr("Gone");
        case 411: return ttstr("Length Required");
        case 412: return ttstr("Precondition Failed");
        case 413: return ttstr("Request Entity Too Large");
        case 414: return ttstr("Request-URI Too Long");
        case 415: return ttstr("Unsupported Media Type");
        case 416: return ttstr("Requested Range Not Satisfiable");
        case 417: return ttstr("Expectation Failed");
        case 500: return ttstr("Internal Server Error");
        case 501: return ttstr("Not Implemented");
        case 502: return ttstr("Bad Gateway");
        case 503: return ttstr("Service Unavailable");
        case 504: return ttstr("Gateway Timeout");
        case 505: return ttstr("HTTP Version Not Supported");
        default: return ttstr("");
        }
    }

    const std::vector<char>* GetResponseText(void) const {
        RaiseExceptionIfNotResponsed();

        return &_responseBody;
    }

    void Open(const ttstr &method, const ttstr &uri, bool async, const ttstr &username, const ttstr &password)
    {
        Initialize();

        if (method == ttstr(TJS_W("GET"))) {
            _method = std::string("GET");
        }
        else if (method == ttstr(TJS_W("POST"))) {
            _method = std::string("POST");
        }
        else {
            TVPThrowExceptionMessage(TJS_W("SYNTAX_ERR (Wrong method)"));
            return;
        }

        _async = async;

        boost::reg_expression<tjs_char> re(
            ttstr("http://"
                  "("
                  "(?:[a-zA-Z0-9](?:[-a-zA-Z0-9]*[a-zA-Z0-9]|(?:))\\.)*[a-zA-Z](?:[-a-zA-Z0-9]*[a-zA-Z0-9]|(?:))\\.?" // hostname
                  "|"
                  "[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+" // IPv4addr
                  ")"
                  "(?::([0-9]+))?" // port
                  "(.*)").c_str(),
            boost::regbase::normal|boost::regbase::use_except|boost::regbase::nocollate);
        boost::match_results<const tjs_char*> what;
        bool matched = boost::regex_search(uri.c_str(), what, re, boost::match_default);

        if (!matched) {
            TVPThrowExceptionMessage(TJS_W("Wrong URL"));
            return;
        }

        _host = "";
        _host.append(what[1].first, what[1].second);

        if (what[2].matched) {
            _port = TJSStringToInteger(ttstr(what[2].first, what[2].second - what[2].first).c_str());
        }
        else {
            _port = 80;
        }
        
        _path = "";
        _path.append(what[3].first, what[3].second);

        if (username.GetLen() > 0) {
            if (IsValidUserInfo(username, password)) {
                std::string authKey = "";
                std::copy(username.c_str(), username.c_str() + username.length(), std::back_inserter(authKey));
                authKey.append(":");
                std::copy(password.c_str(), password.c_str() + password.length(), std::back_inserter(authKey));

                _requestHeaders.insert(std::pair<std::string, std::string>("Authorization", std::string("Basic ") + EncodeBase64(authKey)));
            }
            else {
                TVPThrowExceptionMessage(TJS_W("Wrong UserInfo"));
                return;
            }
        }

        SetReadyState(1);
    }

    void Send(tTJSVariant *data)
    {
        if (_hThread) {
            TVPAddLog(ttstr("レスポンスが戻る前に send しました"));
            return;
        }

        if (_readyState != 1) {
            TVPThrowExceptionMessage(TJS_W("INVALID_STATE_ERR"));
            return;
        }

        _aborted = false;

        _sendingData.clear();
        if (data) {
            tTJSVariantOctet *oct = data->AsOctetNoAddRef();
            _sendingData.reserve(oct->GetLength());
            std::copy(oct->GetData(), oct->GetData() + oct->GetLength(), std::back_inserter(_sendingData));
            std::string slen = boost::lexical_cast<std::string>(_sendingData.size());
            _requestHeaders.insert(std::pair<std::string, std::string>("Content-Length", slen));
        }

        std::string hostHeader = _port == 80 ? _host : _host + ":" + boost::lexical_cast<std::string>(_port);
        _requestHeaders.insert(std::pair<std::string, std::string>("Host", hostHeader));

        if (_async) {
            _hThread = (HANDLE)_beginthreadex(NULL, 0, StartProc, this, 0, NULL);
        }
        else {
            _Send();
        }
    }

    void _Send(void)
    {
        _responseData.clear();

        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            OnErrorOnSending();
            return;
        }

        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(_port);
        server.sin_addr.S_un.S_addr = inet_addr(_host.c_str());

        if (server.sin_addr.S_un.S_addr == 0xffffffff) {
            hostent *hst;
            hst = gethostbyname(_host.c_str());
            if (!hst) {
                OnErrorOnSending();
                return;
            }

            unsigned int **addrptr;
            addrptr = (unsigned int **)hst->h_addr_list;

            while (*addrptr) {
                server.sin_addr.S_un.S_addr = **addrptr;
                if (!connect(sock, (struct sockaddr *)&server, sizeof(server))) {
                    break;
                }
                ++addrptr;
            }

            if (!*addrptr) {
                OnErrorOnSending();
                return;
            }
        }
        else {
            if (connect(sock, (struct sockaddr *)&server, sizeof(server))) {
                OnErrorOnSending();
                return;
            }
        }

        std::ostringstream req;
        std::vector<char> reqv;
        std::string reqstr;
        char buf[4096];
        int n;

        if (_aborted) {
            goto onaborted;
        }

        req << _method << " " << _path << " HTTP/1.1\r\n";
        for (header_container::const_iterator p = _requestHeaders.begin(); p != _requestHeaders.end(); ++p) {
            req << p->first << ": " << p->second << "\r\n";
        }
        req << "\r\n";

        reqstr = req.str();
        std::copy(reqstr.begin(), reqstr.end(), std::back_inserter(reqv));

        if (!_sendingData.empty()) {
            std::copy(_sendingData.begin(), _sendingData.end(), std::back_inserter(reqv));
        }

        n = send(sock, &reqv[0], reqv.size(), 0);
        SetReadyState(2);
        if (n < 0) {
            OnErrorOnSending();
            return;
        }

        SetReadyState(3);

        fd_set fds, readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 50 * 1000;
        while (!_aborted && n > 0) {
            memcpy(&fds, &readfds, sizeof(fd_set));
            while (select(0, &fds, NULL, NULL, &tv) == 0) {
                if (_aborted) {
                    goto onaborted;
                }
            }

            memset(buf, 0, sizeof(buf));
            n = recv(sock, buf, sizeof(buf), 0);
            _responseData.insert(_responseData.end(), buf, buf + n);
            if (n < 0) {
                OnErrorOnSending();
                return;
            }
        }

    onaborted:
        closesocket(sock);

        if (_hThread) {
            CloseHandle(_hThread);
        }
        _hThread = NULL;

        if (!_aborted) {
            ParseResponse();
            SetReadyState(4);
        }
    }

    void OnReadyStateChange() {
        EnterCriticalSection(&_criticalSection);

        if (_async && _target->IsValid(TJS_IGNOREPROP, L"onreadystatechange", NULL, _target) == TJS_S_TRUE) {
            _readyStateChangeQueue.push_back(_readyState);
        }

        LeaveCriticalSection(&_criticalSection);
    }

    void ExecuteOnReadyStateChangeCallback(void) {
        EnterCriticalSection(&_criticalSection);
        Sleep(1);
        for (std::vector<tjs_int>::const_iterator p = _readyStateChangeQueue.begin(); p != _readyStateChangeQueue.end(); ++p) {
            tTJSVariant val;
            if (_target->PropGet(TJS_IGNOREPROP, L"onreadystatechange", NULL, &val, _target) < 0) {
                ttstr msg = TJS_W("can't get member: onreadystatechange");
                TVPThrowExceptionMessage(msg.c_str());
            }

            try {
                tTJSVariant v = _target;
                tTJSVariant *param[] = { &v };
                tTJSVariantClosure funcval = val.AsObjectClosureNoAddRef();
                funcval.FuncCall(0, NULL, NULL, NULL, 1, param, NULL);
            }
            catch (...) {
                LeaveCriticalSection(&_criticalSection);
                throw;
            }
        }
        _readyStateChangeQueue.clear();

        LeaveCriticalSection(&_criticalSection);
    }


    static unsigned __stdcall StartProc(void *arg)
    {
        ((NI_XMLHttpRequest*)arg)->_Send();
        return 0;
    }

    /*
     * マージせずに単に新しい値で上書きする
     */
    void SetRequestHeader(const ttstr &header, const ttstr &value)
    {
        if (_readyState != 1) {
            TVPThrowExceptionMessage(TJS_W("INVALID_STATE_ERR"));
        }

        if (!IsValidHeaderName(header)) {
            TVPThrowExceptionMessage(TJS_W("SYNTAX_ERR"));
        }

        if (!IsValidHeaderValue(value)) {
            TVPThrowExceptionMessage(TJS_W("SYNTAX_ERR"));
        }

        std::string sheader;
        std::string svalue;
        std::copy(header.c_str(), header.c_str() + header.length(), std::back_inserter(sheader));
        std::copy(value.c_str(), value.c_str() + value.length(), std::back_inserter(svalue));

        _requestHeaders.erase(sheader);
        _requestHeaders.insert(std::pair<std::string, std::string>(sheader, svalue));
    }

    // for debug
    void PrintRequestHeaders(void)
    {
        for (header_container::const_iterator p = _requestHeaders.begin(); p != _requestHeaders.end(); ++p) {
            TVPAddLog((p->first + ": " + p->second).c_str());
        }
    }

    void Abort(void)
    {
        _aborted = true;

        if (_hThread) {
            WaitForSingleObject(_hThread, INFINITE);
        }

        Initialize();
    }

private:
    void OnErrorOnSending()
    {
        _readyState = 4;
    }

    void RaiseExceptionIfNotResponsed(void) const
    {
        if (_readyState != 3 && _readyState != 4) {
            TVPThrowExceptionMessage(TJS_W("INVALID_STATE_ERR"));
        }
    }

    bool IsValidUserInfo(const ttstr &username, const ttstr &password)
    {
        if (username.length() == 0 || password.length() == 0) {
            return true;
        }

        return std::find_if(username.c_str(), username.c_str() + username.length(), NI_XMLHttpRequest::IsInvalidUserInfoCharacter) ==
            username.c_str() + username.length() &&
            std::find_if(password.c_str(), password.c_str() + password.length(), NI_XMLHttpRequest::IsInvalidUserInfoCharacter) ==
            password.c_str() + password.length();
    }

    static bool IsInvalidUserInfoCharacter(tjs_char c)
    {
        return c > 127; // non US-ASCII character
    }

    bool IsValidHeaderName(const ttstr &header)
    {
        return header.length() > 0 &&
            std::find_if(header.c_str(), header.c_str() + header.length(), NI_XMLHttpRequest::IsInvalidHeaderNameCharacter) ==
            header.c_str() + header.length();
    }

    static bool IsInvalidHeaderNameCharacter(tjs_char c)
    {
        if (c > 127) return true; // non US-ASCII character
        if (c <= 31 || c == 127) return true; // CTL
        const std::string separators = "()<>@,;:\\\"/[]?={} \t";
        return std::find(separators.begin(), separators.end(), c) != separators.end();
    }

    bool IsValidHeaderValue(const ttstr &value)
    {
        // 単純のため "\r\n" は許さないことにする
        if (wcsstr(value.c_str(), L"\r\n")) {
            return false;
        }

        return true;
    }

    void ParseResponse()
    {
        boost::reg_expression<char> re("\\AHTTP/[0-9]+\\.[0-9]+ ([0-9][0-9][0-9])",
            boost::regbase::normal|boost::regbase::use_except|boost::regbase::nocollate);
        boost::match_results<const char*> what;
        bool matched = boost::regex_search(_responseData.begin(), what, re, boost::match_default);

        if (matched) {
            std::string s(what[1].first, what[1].second - what[1].first);
            _responseStatus = boost::lexical_cast<int>(s);
        }

        _responseBody.clear();
        std::string sep("\r\n\r\n");
        std::vector<char>::iterator p = std::search(_responseData.begin(), _responseData.end(), sep.begin(), sep.end());
        if (p != _responseData.end()) {
            _responseBody.reserve(_responseData.end() - p - sep.size());
            std::copy(p + sep.size(), _responseData.end(), std::back_inserter(_responseBody));
        }
    }

    std::string EncodeBase64(const std::string target)
    {
        std::string result = "";
        std::vector<char> r;

        int len, restlen;
        len = restlen = target.length();

        while (restlen >= 3) {
            char t1 = target[len - restlen];
            char t2 = target[len - restlen + 1];
            char t3 = target[len - restlen + 2];

            r.push_back(t1 >> 2);
            r.push_back(((t1 & 3) << 4) | (t2 >> 4));
            r.push_back(((t2 & 0x0f) << 2) | (t3 >> 6));
            r.push_back(t3 & 0x3f);

            restlen -= 3;
        }

        if (restlen == 1) {
            char t1 = target[len - restlen];
            char t2 = '\0';
            r.push_back(t1 >> 2);
            r.push_back(((t1 & 3) << 4) | (t2 >> 4));

        }
        else if (restlen == 2) {
            char t1 = target[len - restlen];
            char t2 = target[len - restlen + 1];
            char t3 = '\0';
            r.push_back(t1 >> 2);
            r.push_back(((t1 & 3) << 4) | (t2 >> 4));
            r.push_back(((t2 & 0x0f) << 2) | (t3 >> 6));
        }

        for (std::vector<char>::const_iterator p = r.begin(); p != r.end(); ++p) {
            result.append(1, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[*p]);
        }

        if (restlen > 0) {
            result.append(3 - restlen, '=');
        }

        return result;
    }
private:
    tjs_int _readyState;
    std::string _method;
    bool _async;
    int _port;
    std::string _host;
    std::string _path;
    std::vector<char> _responseData;
    std::vector<char> _responseBody;
    int _responseStatus;
    std::vector<tjs_uint8> _sendingData;

    typedef std::map<std::string, std::string> header_container;
    header_container _requestHeaders;
    HANDLE _hThread;
    bool _aborted;
    iTJSDispatch2 *_target;
    std::vector<tjs_int> _readyStateChangeQueue;
    CRITICAL_SECTION _criticalSection;

    static int objcount;
};

int NI_XMLHttpRequest::objcount = 0;

//---------------------------------------------------------------------------
/*
    これは NI_XMLHttpRequest のオブジェクトを作成して返すだけの関数です。
    後述の TJSCreateNativeClassForPlugin の引数として渡します。
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_XMLHttpRequest()
{
    return new NI_XMLHttpRequest();
}
//---------------------------------------------------------------------------
/*
    TJS2 のネイティブクラスは一意な ID で区別されている必要があります。
    これは後述の TJS_BEGIN_NATIVE_MEMBERS マクロで自動的に取得されますが、
    その ID を格納する変数名と、その変数をここで宣言します。
    初期値には無効な ID を表す -1 を指定してください。
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_XMLHttpRequest
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
    TJS2 用の「クラス」を作成して返す関数です。
*/
static iTJSDispatch2 * Create_NC_XMLHttpRequest()
{
    /*
        まず、クラスのベースとなるクラスオブジェクトを作成します。
        これには TJSCreateNativeClassForPlugin を用います。
        TJSCreateNativeClassForPlugin の第１引数はクラス名、第２引数は
        ネイティブインスタンスを返す関数を指定します。
        作成したオブジェクトを一時的に格納するローカル変数の名前は
        classobj である必要があります。
    */
    tTJSNativeClassForPlugin * classobj =
        TJSCreateNativeClassForPlugin(TJS_W("XMLHttpRequest"), Create_NI_XMLHttpRequest);


    /*
        TJS_BEGIN_NATIVE_MEMBERS マクロです。引数には TJS2 内で使用するクラス名
        を指定します。
        このマクロと TJS_END_NATIVE_MEMBERS マクロで挟まれた場所に、クラスの
        メンバとなるべきメソッドやプロパティの記述をします。
    */
    TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/XMLHttpRequest)

        TJS_DECL_EMPTY_FINALIZE_METHOD


        TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
            /*var.name*/_this,
            /*var.type*/NI_XMLHttpRequest,
            /*TJS class name*/XMLHttpRequest)
        {
            // NI_XMLHttpRequest::Construct にも内容を記述できるので
            // ここでは何もしない
            return TJS_S_OK;
        }
        TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/XMLHttpRequest)


        TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/open)
        {
            TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                /*var. type*/NI_XMLHttpRequest);

            if (numparams < 2) return TJS_E_BADPARAMCOUNT;

            if (param[0]->Type() != tvtString || param[1]->Type() != tvtString) {
                return TJS_E_INVALIDPARAM;
            }

            bool async;
            if (numparams < 3) {
                async = true;
            }
            else {
                async = (bool)*param[2];
            }

            ttstr username;
            if (numparams < 4) {
                username = "";
            }
            else {
                if (param[3]->Type() == tvtString) {
                    username = *param[3];
                }
                else {
                    return TJS_E_INVALIDPARAM;
                }
            }

            ttstr password;
            if (numparams < 5) {
                password = "";
            }
            else {
                if (param[4]->Type() == tvtString) {
                    password = *param[4];
                }
                else {
                    return TJS_E_INVALIDPARAM;
                }
            }

            _this->Open(ttstr(*param[0]), ttstr(*param[1]), async, username, password);

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(/*func. name*/open)

        TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/send)
        {
            TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                /*var. type*/NI_XMLHttpRequest);

            if (numparams == 0 || param[0]->Type() == tvtVoid) {
                _this->Send(NULL);
            }
            else if (param[0]->Type() == tvtOctet) {
                _this->Send(param[0]);
            }
            else {
                return TJS_E_INVALIDPARAM;
            }

            if (result) {
                result->Clear();
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(/*func. name*/send)


        TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setRequestHeader)
        {
            TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                /*var. type*/NI_XMLHttpRequest);

            if (numparams < 2) return TJS_E_BADPARAMCOUNT;
            if (param[0]->Type() != tvtString || param[1]->Type() != tvtString) {
                return TJS_E_INVALIDPARAM;
            }

            _this->SetRequestHeader(ttstr(*param[0]), ttstr(*param[1]));
            if (result) {
                result->Clear();
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(/*func. name*/setRequestHeader)


        TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/printRequestHeaders)
        {
            TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                /*var. type*/NI_XMLHttpRequest);

            _this->PrintRequestHeaders();
            if (result) {
                result->Clear();
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(/*func. name*/printRequestHeaders)


        TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/abort)
        {
            TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                /*var. type*/NI_XMLHttpRequest);

            _this->Abort();
            if (result) {
                result->Clear();
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(/*func. name*/abort)

        TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/executeCallback)
        {
            TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                /*var. type*/NI_XMLHttpRequest);

            _this->ExecuteOnReadyStateChangeCallback();
            if (result) {
                result->Clear();
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(/*func. name*/executeCallback)

        TJS_BEGIN_NATIVE_PROP_DECL(readyState)
        {
            TJS_BEGIN_NATIVE_PROP_GETTER
            {
                TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                    /*var. type*/NI_XMLHttpRequest);

                if (result) {
                    *result = (tTVInteger)_this->GetReadyState();
                }

                return TJS_S_OK;
            }
            TJS_END_NATIVE_PROP_GETTER

            TJS_DENY_NATIVE_PROP_SETTER
        }
        TJS_END_NATIVE_PROP_DECL(readyState)


        TJS_BEGIN_NATIVE_PROP_DECL(responseText)
        {
            TJS_BEGIN_NATIVE_PROP_GETTER
            {
                TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                    /*var. type*/NI_XMLHttpRequest);

                if (result) {
                    const std::vector<char>* data = _this->GetResponseText();
                    tjs_uint8 *d = new tjs_uint8[data->size()];
                    std::copy(data->begin(), data->end(), d);
                    *result = TJSAllocVariantOctet(d, data->size());
                    delete[] d;
                }

                return TJS_S_OK;
            }
            TJS_END_NATIVE_PROP_GETTER

            TJS_DENY_NATIVE_PROP_SETTER
        }
        TJS_END_NATIVE_PROP_DECL(responseText)



        TJS_BEGIN_NATIVE_PROP_DECL(status)
        {
            TJS_BEGIN_NATIVE_PROP_GETTER
            {
                TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                    /*var. type*/NI_XMLHttpRequest);

                if (result) {
                    *result = (tTVInteger)_this->GetResponseStatus();
                }

                return TJS_S_OK;
            }
            TJS_END_NATIVE_PROP_GETTER

            TJS_DENY_NATIVE_PROP_SETTER
        }
        TJS_END_NATIVE_PROP_DECL(status)

        TJS_BEGIN_NATIVE_PROP_DECL(statusText)
        {
            TJS_BEGIN_NATIVE_PROP_GETTER
            {
                TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
                    /*var. type*/NI_XMLHttpRequest);

                if (result) {
                    *result = _this->GetResponseStatusText();
                }

                return TJS_S_OK;
            }
            TJS_END_NATIVE_PROP_GETTER

            TJS_DENY_NATIVE_PROP_SETTER
        }
        TJS_END_NATIVE_PROP_DECL(statusText)

    TJS_END_NATIVE_MEMBERS

    /*
        この関数は classobj を返します。
    */
    return classobj;
}
//---------------------------------------------------------------------------
/*
    TJS_NATIVE_CLASSID_NAME は一応 undef しておいたほうがよいでしょう
*/
#undef TJS_NATIVE_CLASSID_NAME
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
    void* lpReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
    // スタブの初期化(必ず記述する)
    TVPInitImportStub(exporter);

    tTJSVariant val;

    // TJS のグローバルオブジェクトを取得する
    iTJSDispatch2 * global = TVPGetScriptDispatch();


    //-----------------------------------------------------------------------
    // 1 まずクラスオブジェクトを作成
    iTJSDispatch2 * tjsclass = Create_NC_XMLHttpRequest();

    // 2 tjsclass を tTJSVariant 型に変換
    val = tTJSVariant(tjsclass);

    // 3 すでに val が tjsclass を保持しているので、tjsclass は
    //   Release する
    tjsclass->Release();


    // 4 global の PropSet メソッドを用い、オブジェクトを登録する
    global->PropSet(
        TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
        TJS_W("XMLHttpRequest"), // メンバ名 ( かならず TJS_W( ) で囲む )
        NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
        &val, // 登録する値
        global // コンテキスト ( global でよい )
        );
    //-----------------------------------------------------------------------


    // - global を Release する
    global->Release();

    // もし、登録する関数が複数ある場合は 1 ～ 4 を繰り返す


    // val をクリアする。
    // これは必ず行う。そうしないと val が保持しているオブジェクト
    // が Release されず、次に使う TVPPluginGlobalRefCount が正確にならない。
    val.Clear();


    // この時点での TVPPluginGlobalRefCount の値を
    GlobalRefCountAtInit = TVPPluginGlobalRefCount;
    // として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
    // 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
    // 解放時にはこれと同じか、これよりも少なくなってないとならない。
    // そうなってなければ、どこか別のところで関数などが参照されていて、
    // プラグインは解放できないと言うことになる。

    return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
    // 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数。

    // もし何らかの条件でプラグインを解放できない場合は
    // この時点で E_FAIL を返すようにする。
    // ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
    // 大きくなっていれば失敗ということにする。
    if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
        // E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す


    /*
        ただし、クラスの場合、厳密に「オブジェクトが使用中である」ということを
        知るすべがありません。基本的には、Plugins.unlink によるプラグインの解放は
        危険であると考えてください (いったん Plugins.link でリンクしたら、最後ま
        でプラグインを解放せず、プログラム終了と同時に自動的に解放させるのが吉)。
    */

    // TJS のグローバルオブジェクトに登録した XMLHttpRequest クラスなどを削除する

    // - まず、TJS のグローバルオブジェクトを取得する
    iTJSDispatch2 * global = TVPGetScriptDispatch();

    // - global の DeleteMember メソッドを用い、オブジェクトを削除する
    if(global)
    {
        // TJS 自体が既に解放されていたときなどは
        // global は NULL になり得るので global が NULL でない
        // ことをチェックする

        global->DeleteMember(
            0, // フラグ ( 0 でよい )
            TJS_W("XMLHttpRequest"), // メンバ名
            NULL, // ヒント
            global // コンテキスト
            );
    }

    // - global を Release する
    if(global) global->Release();

    // スタブの使用終了(必ず記述する)
    TVPUninitImportStub();

    return S_OK;
}
//---------------------------------------------------------------------------

