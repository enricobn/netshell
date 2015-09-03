/*
 * HttpStaticHandler.h
 *
 *  Created on: May 30, 2014
 *      Author: enrico
 */

#ifndef HTTPSTATICHANDLER_H_
#define HTTPSTATICHANDLER_H_

#include <string>
#include "HttpHandler.h"

class HttpStaticHandler : public HttpHandler {
private:
	std::string path;
public:
	HttpStaticHandler(std::string path);

	void on_http_request(HttpRequest* request, HttpResponse* response);

	void on_websocket_connect(HttpRequest* request, SocketConnection* connection);

	void on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data);

	void on_websocket_close(HttpRequest* request, SocketConnection* connection);

	virtual ~HttpStaticHandler();
};

#endif /* HTTPSTATICHANDLER_H_ */
