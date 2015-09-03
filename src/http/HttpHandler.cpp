/*
 * IHttpHandler.cpp
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#include "HttpHandler.h"

HttpHandler::HttpHandler() :
application(NULL)
{
}

void HttpHandler::set_path(std::string path) {
	this->path = path;
}

void HttpHandler::set_application(HttpApplication* application) {
	this->application = application;
}

HttpApplication* HttpHandler::get_application() {
	return application;
}

std::string HttpHandler::get_absolute_path(std::string path) {
	std::string result = "/" + application->get_path();
	if (get_path() != "*") {
		result += "/" + get_path();
	}
	return result + "/" + path;
}

std::string HttpHandler::get_path() {
	return path;
}

void HttpHandler::on_http_request(HttpRequest* request, HttpResponse* response) {
}

void HttpHandler::on_websocket_connect(HttpRequest* request, SocketConnection* connection) {
}

void HttpHandler::on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data) {
}

void HttpHandler::on_websocket_close(HttpRequest* request, SocketConnection* connection) {
}

HttpHandler::~HttpHandler() {
}
