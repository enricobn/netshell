/*
 * WebSocketRequest.h
 *
 *  Created on: Apr 14, 2014
 *      Author: enrico
 */

#ifndef WEBSOCKETMESSAGEFRAME_H_
#define WEBSOCKETMESSAGEFRAME_H_

#include "../IRequest.h"
#include "../IResponse.h"
#include "../SocketConnection.h"

#include <string>

#include <stddef.h>
#include <stdint.h>

union ShortHeader {
    struct {
      unsigned int OP_CODE : 4;
      unsigned int RSV1 : 1;
      unsigned int RSV2 : 1;
      unsigned int RSV3 : 1;
      unsigned int FIN : 1;
      unsigned int PAYLOAD : 7;
      unsigned int MASK : 1;
    } bits;
    uint16_t all;
};

class WebSocketMessageFrame : public IRequest, IResponse {
private:
	enum State {
		SHORT_HEADER,
		PAYLOAD_16,
		PAYLOAD_64,
		MASK,
		BODY
	};

	ShortHeader short_header;
	State state;
	char buffer[12];
	int buffer_index;
	uint64_t payload;
	std::string body;
	char mask[4];
	int mask_count;
public:
	WebSocketMessageFrame();

	WebSocketMessageFrame(std::string& body, bool final);

	virtual bool add(char* data, int len);

	virtual void send(SocketConnection* connection);

	bool is_final();

	bool is_close();

	std::string* get_body();

	virtual ~WebSocketMessageFrame();
};

#endif /* WEBSOCKETMESSAGEFRAME_H_ */
