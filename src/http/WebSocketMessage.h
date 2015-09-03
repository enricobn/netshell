/*
 * WebSocketRequest.h
 *
 *  Created on: Apr 14, 2014
 *      Author: enrico
 */

#ifndef WEBSOCKETMESSAGE_H_
#define WEBSOCKETMESSAGE_H_

#include "../IRequest.h"
#include "../IResponse.h"
#include "../SocketConnection.h"
#include "WebSocketMessageFrame.h"

#include <vector>

class WebSocketMessage : public IRequest, IResponse {
private:
	WebSocketMessageFrame* current;
	std::string body;
	bool close;
public:
	WebSocketMessage();

	WebSocketMessage(std::string& body);

	virtual bool add(char* data, int len);

	virtual void send(SocketConnection* connection);

	std::string* get_body();

	bool is_close();

	virtual ~WebSocketMessage();
};

#endif /* WEBSOCKETMESSAGE_H_ */
