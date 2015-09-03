/*
 * IHttpHandler.h
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#ifndef IHTTPHANDLER_H_
#define IHTTPHANDLER_H_

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpApplication.h"
#include "../SocketConnection.h"

#include <string>


//TODO rename to IHttpListener
class HttpApplication;

class HttpHandler {
	friend class HttpApplication;
private:
	std::string path;
	HttpApplication* application;

	void set_path(std::string path);

	void set_application(HttpApplication* application);

protected:
	HttpApplication* get_application();

public:
	HttpHandler();

	/**
	 * returns the path obtained from the application's root. Examples:
	 * application: netshell
	 * handler: terminal
	 * path: download
	 * returns: /netshell/terminal/download
	 *
	 * application: netshell
	 * handler: *
	 * path: download
	 * returns: /netshell/download
	 */
	std::string get_absolute_path(std::string path);

	/**
	 * the path relative to the application's root. Example:
	 * application: netshell
	 * handler: terminal
	 * returns: terminal
	 */
	std::string get_path();

	virtual void on_http_request(HttpRequest* request, HttpResponse* response);

	virtual void on_websocket_connect(HttpRequest* request, SocketConnection* connection);

	virtual void on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data);

	virtual void on_websocket_close(HttpRequest* request, SocketConnection* connection);

	virtual ~HttpHandler();
};

#endif /* IHTTPHANDLER_H_ */
