/*
 * IServerMessage.h
 *
 *  Created on: Apr 11, 2014
 *      Author: enrico
 */

#ifndef IRESPONSE_H_
#define IRESPONSE_H_

#include "SocketConnection.h"

class IResponse {
public:
	virtual void send(SocketConnection* connection) = 0;

	virtual ~IResponse();
};

#endif /* IRESPONSE_H_ */
