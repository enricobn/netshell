/*
 * HttpTerminal.h
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#ifndef HTTPTERMINAL_H_
#define HTTPTERMINAL_H_

#include <map>
#include <string>
#include "HttpTerminalSession.h"
#include "http/HttpHandler.h"
#include "terminal/Terminal.h"
#include "terminal/ITerminalListener.h"

class HttpTerminalSession;

class HttpTerminal : public HttpHandler {
private:
	struct DownloadResource {
		std::string path;
		// this is the name which will result to the client, since path is a temp file
		std::string name;
	};
	std::map<SocketConnection*, HttpTerminalSession*> sessions;

	/**
	 * the map where are stored resources to download from within the terminal
	 * key = id of the resource
	 */
	std::map<std::string, DownloadResource> resources;
public:
	HttpTerminal();

	void on_http_request(HttpRequest* request, HttpResponse* response);

	void on_websocket_connect(HttpRequest* request, SocketConnection* connection);

	void on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data);

	void on_websocket_close(HttpRequest* request, SocketConnection* connection);

	void on_attr_change(bool icrnl);

	std::string add_resource(std::string path, std::string name);

	std::string get_handler_path();

	virtual ~HttpTerminal();
};

#endif /* HTTPTERMINAL_H_ */
