#include "serv.hpp"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MediaType.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/CountingStream.h"
#include "Poco/NullStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Exception.h"
#include <iostream>

using Poco::Net::SocketAddress;
using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::MessageHeader;
using Poco::Net::MediaType;
using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;

struct PwRequestCallback {
	typedef PwHTTPServer::RequestCallback Callback;
	PwRequestCallback(Callback _cb, void *_param)   : cb(   _cb), param(   _param) {}
	PwRequestCallback(const PwRequestCallback &src) : cb(src.cb), param(src.param) {}
	void invoke(PwRequestResponse *rr) const { cb(rr, param); }
private:
	Callback cb;
	void *param;
};

struct PwRequestResponseImpl : public PwRequestResponse
{
	PwRequestResponseImpl(HTTPServerRequest &_request, HTTPServerResponse& _response)
		: request(_request), response(_response)
	{
//		response.setChunkedTransferEncoding(true);
		method = request.getMethod();
		uri    = request.getURI();
		try {
			host = request.getHost();
		} catch (Poco::Exception) {
		} catch (...) {
			throw;
		}
		SocketAddress address(request.clientAddress());
		client = address.toString();
	}
	char const *getMethod() const { return method.c_str(); }
	char const *getURI()    const { return uri.c_str(); }
	char const *getHost()   const { return host.c_str(); }
	char const *getClient() const { return client.c_str(); }

	int  getHeader(NameValueCallback cb, void *param) const {
		NameValueCollection::ConstIterator it = request.begin();
		NameValueCollection::ConstIterator end = request.end();
		int cnt = 0;
		for (; it != end; ++it,cnt++) cb(it->first, it->second, param);
		return cnt;
	}
	int  getFormData(NameValueCallback cb, void *param) const {
		HTMLForm form(request, request.stream());
		NameValueCollection::ConstIterator it  = form.begin();
		NameValueCollection::ConstIterator end = form.end();
		int cnt = 0;
		for (; it != end; ++it,cnt++) cb(it->first, it->second, param);
		return cnt;
	}
	inline HTTPResponse::HTTPStatus strToStatus(char const *status) {
		HTTPResponse stat;
		stat.setStatus(status);
		return stat.getStatus();
	}
	void setStatus(char const *status)            { response.setStatusAndReason(strToStatus(status)); }
	void setContentType(char const *type)         { response.setContentType(type); this->type = type; }
	void setRedirect(char const *target)          { response.redirect(target);                        }
	void sendBuffer(void const *buf, Size length) { response.sendBuffer(buf, length);                 }
	void sendFile(char const *path)               { response.sendFile(path, type);                    }
	const String getCharset(char const *mediatype) {
		MediaType mt(mediatype);
		String ret;
		try {
			ret = mt.getParameter("charset");
		} catch (Poco::Exception) {
		} catch (...) {
			throw;
		}
		return ret;
	}
	const String getReason(char const *status) {
		HTTPResponse reason(strToStatus(status));
		return reason.getReason();
	}
private:
	HTTPServerRequest  &request;
	HTTPServerResponse &response;
	String type, method, uri, host, client;
};

struct PwRequestHandler : public HTTPRequestHandler
{
	PwRequestHandler(PwRequestCallback const &_cb) : cb(_cb) {}
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
		PwRequestResponseImpl rr(request, response);
		cb.invoke(&rr);
	}
private:
	PwRequestCallback const cb;
};

struct PwRequestFactory : public HTTPRequestHandlerFactory
{
	PwRequestFactory(PwRequestCallback const &_cb) : cb(_cb) {}
	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) {
		return new PwRequestHandler(cb);
	}
private:
	PwRequestCallback const cb;
};

struct PwHTTPServerImpl : public PwHTTPServer
{
	PwHTTPServerImpl(PwRequestCallback const &_cb) : cb(_cb), socket(0), server(0) {}
	~PwHTTPServerImpl() { stop(); }

	int start(int port) {
		stop();
		SocketAddress sock("127.0.0.1", port);
		socket = new ServerSocket(sock);
		server = new HTTPServer(new PwRequestFactory(cb), *socket, new HTTPServerParams());
		server->start();
		return (int)socket->address().port();
	}
	void stop() {
		if (server == NULL) return;
		server->stop();
		delete server;
		delete socket;
		server = NULL;
		socket = NULL;
	}
private:
	PwRequestCallback const cb;
	ServerSocket *socket;
	HTTPServer   *server;
};

PwHTTPServer*
PwHTTPServer::Factory(PwHTTPServer::RequestCallback callback, void *param) {
	PwRequestCallback cb(callback, param);
	return new PwHTTPServerImpl(cb);
}

