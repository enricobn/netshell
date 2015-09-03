/*
 * HttpTerminalSession.cpp
 *
 *  Created on: May 7, 2014
 *      Author: enrico
 */

#include "HttpTerminalSession.h"
#include "http/WebSocketMessage.h"
#include "JSONParser.h"
#include "JSONObject.h"

#include <iostream>

HttpTerminalSession::HttpTerminalSession(HttpTerminal* http_terminal, SocketConnection* websocket_connection) :
websocket_connection(websocket_connection),
http_terminal(http_terminal)
{
	//	    rc = execlp("bc", "bc", NULL);
	//	    rc = execlp("login", "login", NULL);
    //	    rc = execlp("vim", "vim", NULL);
	terminal = NULL;
//	state = NORMAL;
//	response = "";
	terminalOutputCommandReader = TerminalCommandReader();
	userInputCommandReader = TerminalCommandReader();
}

void HttpTerminalSession::on_terminal_output(const char* buffer, size_t size) {
	terminalOutputCommandReader.on_data_read(buffer, size);

	std::string response = "";

	while (!terminalOutputCommandReader.get_commands()->empty()) {
		JSONObject object = terminalOutputCommandReader.get_commands()->front();
		if (object.get_string("op") == "download") {
			std::string id = http_terminal->add_resource(object.get_string("tmp_file"),
					object.get_string("name"));
			JSONObject new_object = JSONObject();
			new_object.add("op", "download");
			std::string path = http_terminal->get_path();
			if (path.length() > 0) {
				path += "/";
			}
			new_object.add("resource", path + "download/" + id);

			response += "\033_" + new_object.to_string() + "\033\\";
		}
		terminalOutputCommandReader.get_commands()->pop();
	}

	response += terminalOutputCommandReader.pop_current();

	if (response.length() > 0) {
		WebSocketMessage message = WebSocketMessage(response);
		message.send(websocket_connection);
		response = "";
	}
}

void HttpTerminalSession::on_attr_change(bool icrnl) {
/*	std::string response("&c:icrnl=");
	if (icrnl) {
		response += "true";
	} else {
		response += "false";
	}
	WebSocketMessage message = WebSocketMessage(response);
	message.send(websocket_connection);
*/
}

void HttpTerminalSession::on_user_input(const char* buffer, size_t size) {
	userInputCommandReader.on_data_read(buffer, size);

	while (!userInputCommandReader.get_commands()->empty()) {
		JSONObject object = userInputCommandReader.get_commands()->front();
		if (object.get_string("op") == "user") {
			if (terminal != NULL) {
				std::cerr << "terminal already created." << std::endl;
				throw std::exception();
			}
			std::string user = object.get_string("user");
			try {
				terminal = new Terminal(this, "ssh", 4, "ssh", "-l", user.c_str(), "localhost");
				terminal->start();
			} catch(std::exception& e) {
				terminal = NULL;
			}
		}
		userInputCommandReader.get_commands()->pop();
	}

	std::string current = userInputCommandReader.pop_current();
	if (current.length() > 0) {
		if (terminal == NULL) {
			std::cerr << "error sending data to not initialized terminal." << std::endl;
			throw std::exception();
		}
		terminal->write(current.c_str(), current.size());
	}
}

HttpTerminalSession::~HttpTerminalSession() {
	if (terminal != NULL) {
		terminal->remove_listener();
		delete(terminal);
	}
}

