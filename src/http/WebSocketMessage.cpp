/*
 * WebSocketRequest.cpp
 *
 *  Created on: Apr 14, 2014
 *      Author: enrico
 */

#include "WebSocketMessage.h"
#include "WebSocketMessageFrame.h"

#include <vector>

#include "../utils.h"

#include <iostream>

WebSocketMessage::WebSocketMessage() :
current(NULL),
body(""),
close(false)
{

}

WebSocketMessage::WebSocketMessage(std::string& body) :
current(NULL),
body(body),
close(false)
{

}

bool WebSocketMessage::is_close() {
	return close;
}

bool WebSocketMessage::add(char* data, int len) {
	bool result = false;
	if (current == NULL) {
		current = new WebSocketMessageFrame();
	}
	if (current->add(data, len)) {
		close |= current->is_close();
		std::string* frame_body = current->get_body();
		body.append(*frame_body);

		result = current->is_final();

		delete(current);
		current = NULL;
	}
	return result;
}

void WebSocketMessage::send(SocketConnection* connection) {
	WebSocketMessageFrame frame = WebSocketMessageFrame(body, true);
	frame.send(connection);
}

std::string* WebSocketMessage::get_body() {
	return &body;
}

WebSocketMessage::~WebSocketMessage() {
	if( current != NULL) {
		delete(current);
	}
}

