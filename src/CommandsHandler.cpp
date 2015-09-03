/*
 * CommandsHandler.cpp
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#include "CommandsHandler.h"

#include "utils.h"

#include "http/WebSocketMessage.h"

#include <string>
#include <iostream>

std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
    	if (fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

CommandsHandler::CommandsHandler() {
}

void CommandsHandler::on_http_request(HttpRequest* request, HttpResponse* response) {
	if ( request->get_method() == GET )
	{
		if ( request->get_protocol() != HTTP10 && request->get_protocol() != HTTP11 )
		{
			response->set_status(HttpStatus::status_400_Bad_Request);
		}
		else
		{
			if (request->get_uri()->get_path() == "/cmd") {
				std::string cmd = request->get_uri()->get_query().substr(4);

				replace_all(cmd, "%20", " ");

				std::cout << "cmd:" << cmd << std::endl;
				std::string HTML = "<HTML>" + exec(cmd.c_str()) + "</HTML>";

				replace_all(HTML, "\n", "<br>");

				response->set_body(HTML);

			} else {
				std::string file_name;
				// empty URI
				if ( request->get_uri()->get_path() == "/" ) {
					file_name = "/index.html";
				} else {
					file_name = request->get_uri()->get_path();
				}

				#ifdef LOG_RESPONSE
					printf("file: %s\n", file_name.c_str());
				#endif

				// TODO make a get_resource method, but where?
				// Perhaps in an HttpContext in an HttpSession which will be available in HttpRequest
				char cwd[1024];
				if (getcwd(cwd, sizeof(cwd)) != NULL) {
//					fprintf(stdout, "Current working dir: %s\n", cwd);
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

				response->set_file(file_name);
			}
		}
	}
}

void CommandsHandler::on_websocket_request(HttpRequest* request, SocketConnection* connection, std::string* data) {
	std::string response = *data + " to you";
	WebSocketMessage ws_response = WebSocketMessage(response);

	ws_response.send(connection);
}

CommandsHandler::~CommandsHandler() {
}

