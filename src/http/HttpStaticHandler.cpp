/*
 * HttpStaticHandler.cpp
 *
 *  Created on: May 30, 2014
 *      Author: enrico
 */

#include "HttpStaticHandler.h"
#include "HttpStatus.h"
#include "ContentTypeProvider.h"
#include "../FileName.h"
#include <iostream>

HttpStaticHandler::HttpStaticHandler(std::string path) :
path(path)
{
}

void HttpStaticHandler::on_http_request(HttpRequest* request, HttpResponse* response) {
	if ( request->get_method() == GET )
	{
		if ( request->get_protocol() != HTTP10 && request->get_protocol() != HTTP11 )
		{
			response->set_status(HttpStatus::status_400_Bad_Request);
		}
		else
		{
			std::string file_name;
			std::string content_type;
			if ( request->get_uri()->get_path() == "/" + path) {
				file_name = "/" + path + "/index.html";
			} else {
				file_name = request->get_uri()->get_path();
			}
			content_type = ContentTypeProvider::getInstance().get_type(FileName(file_name).get_ext());

			// TODO make a get_resource method, but where?
			// Perhaps in an HttpContext in an HttpSession which will be available in HttpRequest
			char cwd[1024];
			if (getcwd(cwd, sizeof(cwd)) != NULL) {
//				fprintf(stdout, "Current working dir: %s\n", cwd);
			} else {
				perror("getcwd() error");
				//TODO close (Exception )
				exit(1);
			}

			std::string web = std::string(cwd);
			web += "/web";

			file_name = web + file_name;
			#ifdef LOG_RESPONSE
				printf("file: %s\n", file_name.c_str());
			#endif

			response->set_file(file_name, content_type);

		}
	}
}

void HttpStaticHandler::on_websocket_connect(HttpRequest* request, SocketConnection* connection) {
	std::cerr << "HttpStaticHandler: cannot handle websocket." << std::endl;
	throw std::exception();
}

void HttpStaticHandler::on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data) {
	std::cerr << "HttpStaticHandler: cannot handle websocket." << std::endl;
	throw std::exception();
}

void HttpStaticHandler::on_websocket_close(HttpRequest* request, SocketConnection* connection) {
	std::cerr << "HttpStaticHandler: cannot handle websocket." << std::endl;
	throw std::exception();
}

HttpStaticHandler::~HttpStaticHandler() {
}

