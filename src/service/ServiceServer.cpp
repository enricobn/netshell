/*
 * ServiceServer.cpp
 *
 *  Created on: May 1, 2014
 *      Author: enrico
 */

#include "ServiceServer.h"

#include <string>
#include <iostream>
#include "../SocketServer.h"
#include "../SocketConnection.h"

ServiceServer::ServiceServer(std::string port) :
	SocketServer(port),
	listener(NULL),
	request(NULL)
{
}

bool ServiceServer::data_received(SocketConnection* connection, char* data, int len) {
	if (request == NULL) {
		request = new ServiceRequest();
	}
	if (request->add(data, len)) {
		if (listener == NULL) {
			std::cerr << "No service listener" << std::endl;
			throw std::exception();
		}
		listener->on_request(request);
		delete(request);
		request = NULL;
		return true;
	}
	return false;
}

void ServiceServer::set_listener(IServiceListener* listener) {
	this->listener = listener;
}

ServiceServer::~ServiceServer() {
}

