/*
 * ServiceServer.h
 *
 *  Created on: May 1, 2014
 *      Author: enrico
 */

#ifndef SERVICESERVER_H_
#define SERVICESERVER_H_

#include <string>

#include "../SocketServer.h"
#include "ServiceRequest.h"
#include "IServiceListener.h"

class ServiceServer : public SocketServer {
private:
	IServiceListener* listener;
	ServiceRequest* request;

public:
	ServiceServer(std::string port);

	bool data_received(SocketConnection* connection, char* data, int len);

	void set_listener(IServiceListener* listener);

	virtual ~ServiceServer();
};

#endif /* SERVICESERVER_H_ */
