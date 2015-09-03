/*
 * HttpTerminal.cpp
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#include "FileName.h"
#include "HttpTerminal.h"
#include "http/ContentTypeProvider.h"
#include "http/WebSocketMessage.h"

#include <functional>
#include <iostream>
//#include <pthread.h>
//#include <thread>

HttpTerminal::HttpTerminal() :
sessions(std::map<SocketConnection*, HttpTerminalSession*>()),
resources(std::map<std::string, DownloadResource>())
{
}

std::string HttpTerminal::get_handler_path() {
	return get_path();
}

void HttpTerminal::on_http_request(HttpRequest* request, HttpResponse* response) {
	if ( request->get_method() == GET )
	{
		if ( request->get_protocol() != HTTP10 && request->get_protocol() != HTTP11 )
		{
			response->set_status(HttpStatus::status_400_Bad_Request);
		}
		else
		{

			std::cout << "HttpTerminal:" << std::endl;
			std::cout << "    request->get_uri()->get_path()=" << request->get_uri()->get_path() << std::endl;
			std::cout << "    get_path()=" << get_path() << std::endl;
			std::string download_path = get_absolute_path("download");
			std::cout << "    download_path=" << download_path << std::endl;

			std::string file_name;
			std::string content_type;

			if ( request->get_uri()->get_path().substr(0, download_path.length()) == download_path ) {
				std::string id = request->get_uri()->get_path().substr(download_path.length() + 1);
				std::cout << "    id" << id << std::endl;
				DownloadResource resource = resources[id];

				file_name = resource.path;
				content_type = ContentTypeProvider::getInstance().get_type(FileName(resource.name).get_ext());

				response->set_header("Content-Disposition", "attachment; filename=" + resource.name);
				response->set_header("Set-Cookie", "fileDownload=true; path=/");
				// TODO how to delete temp file and release id?
			}

			printf("download file: %s\n", file_name.c_str());


			response->set_file(file_name, content_type);
		}
	}
}

void HttpTerminal::on_websocket_close(HttpRequest* request, SocketConnection* connection) {
	HttpTerminalSession* session = sessions[connection];
	sessions.erase(connection);
	delete(session);
}

void HttpTerminal::on_websocket_connect(HttpRequest* request, SocketConnection* connection) {
	sessions[connection] = new HttpTerminalSession(this, connection);
}

void HttpTerminal::on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data) {
	HttpTerminalSession* session = sessions[connection];
	session->on_user_input(data->c_str(), data->length());
}

std::string HttpTerminal::add_resource(std::string path, std::string name) {
	// TODO
	std::string id = "0";
	resources[id] = {path, name};
	return id;
}

HttpTerminal::~HttpTerminal() {
}

