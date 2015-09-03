/*
 * CommandsHandler.h
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#ifndef COMMANDSHANDLER_H_
#define COMMANDSHANDLER_H_

#include "http/HttpHandler.h"

class CommandsHandler : public HttpHandler {
public:
	CommandsHandler();

	void on_http_request(HttpRequest* request, HttpResponse* response);

//	virtual void on_websocket_connect(HttpRequest* request, SocketConnection* connection);

	void on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data);

//	virtual void on_websocket_close(HttpRequest* request, SocketConnection* connection);


	virtual ~CommandsHandler();
};

#endif /* COMMANDSHANDLER_H_ */
