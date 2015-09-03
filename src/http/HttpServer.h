/*
 * HttpServer.h
 *
 *  Created on: Apr 11, 2014
 *      Author: enrico
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "HttpApplication.h"
#include "HttpRequest.h"
#include "HttpStaticHandler.h"
#include "../IRequest.h"
#include "../IResponse.h"
#include "../SocketConnection.h"
#include "../SocketServer.h"

#include <string>
#include <map>

//typedef void (*Handler)(const IRequest* request, IResponse* response);

class HttpServer : public SocketServer {
private:
	//static const HttpStaticHandler static_handler;
	std::map<SocketConnection*, IRequest*> requests;
	// key = path
	std::map<std::string, HttpApplication*> applications;
	std::map<SocketConnection*, IRequest*> web_sockets;

	HttpHandler* get_handler(HttpRequest* request);
public:
	HttpServer(std::string port);

	/**
	 * return true to close and free the connection
	 * TODO protected
	 */
	virtual bool data_received(SocketConnection* connection, char* data, int len);

	virtual void connection_closed(SocketConnection* connection);

	HttpApplication* create_application(std::string path);

	// TODO private
	IRequest* create_request();

	// TODO private
	IRequest* create_web_socket_request();

	// TODO private
	IResponse* create_response(IRequest* request);

	virtual ~HttpServer();
};

#endif /* HTTPSERVER_H_ */
