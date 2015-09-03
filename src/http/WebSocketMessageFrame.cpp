/*
 * WebSocketRequest.cpp
 *
 *  Created on: Apr 14, 2014
 *      Author: enrico
 */

#include "WebSocketMessageFrame.h"

#include <stdlib.h>
#include <string.h>

#include "../utils.h"

#include <iostream>
/*
 * thanks to:
 * http://www.altdevblogaday.com/2012/01/23/writing-your-own-websocket-server/
 * http://tools.ietf.org/html/rfc6455
 *
 */
WebSocketMessageFrame::WebSocketMessageFrame() :
short_header(ShortHeader()),
state(SHORT_HEADER),
buffer_index(0),
payload(0),
body(""),
mask_count(0)
{

}

//TODO call the default constructor
WebSocketMessageFrame::WebSocketMessageFrame(std::string& body, bool final) :
short_header(ShortHeader()),
state(SHORT_HEADER),
buffer_index(0),
payload(0),
body(""),
mask_count(0)
{
	short_header.bits.FIN = final ? 1 : 0;
	// TODO support for mask
	short_header.bits.MASK = 0;
	// TODO
	short_header.bits.OP_CODE = 1;
	if (body.size() > 65535) {
		short_header.bits.PAYLOAD = 127u;
	} else if (body.size() > 125) {
		short_header.bits.PAYLOAD = 126u;
	} else {
		short_header.bits.PAYLOAD = body.size();
	}
	payload = body.size();
	this->body = body;
}
bool WebSocketMessageFrame::is_close() {
	return short_header.bits.OP_CODE == 8;
}

bool WebSocketMessageFrame::add(char* data, int len) {
	bool result = false;

	for (int i = 0; i < len; i++) {
		char c = data[i];
		if (state == SHORT_HEADER) {
			buffer[buffer_index++] = c;
			if (buffer_index == 2) {
				memcpy(&short_header.all, buffer, 2);

//				// TODO I don't handle fragments
//				if (short_header.bits.FIN == 0) {
//					std::cerr << "I don't handle fragments";
//					throw std::exception();
//				}

				if (short_header.bits.PAYLOAD == 126u) {
					state = PAYLOAD_16;
				} else if (short_header.bits.PAYLOAD == 127u) {
					state = PAYLOAD_64;
				} else {
					payload = short_header.bits.PAYLOAD;
					if (short_header.bits.MASK == 1) {
						state = MASK;
					} else {
						state = BODY;
					}
				}
				buffer_index = 0;
//				std::cout << "short header payload=" << short_header.bits.PAYLOAD << std::endl;
			}
		} else if (state == PAYLOAD_16) {
			buffer[buffer_index++] = c;
			if (buffer_index == 2) {
				uint16_t payload_16 = 0;
				memcpy(&payload_16, buffer, 2);

				buffer_index = 0;

				payload = to_endian_native(payload_16);

				if (short_header.bits.MASK == 1) {
					state = MASK;
				} else {
					state = BODY;
				}
			}
		} else if (state == PAYLOAD_64) {
			buffer[buffer_index++] = c;
			if (buffer_index == 8) {
				uint64_t payload_64 = 0;
				memcpy(&payload_64, buffer, 8);

				buffer_index = 0;

				payload = to_endian_native(payload_64);

				if (short_header.bits.MASK == 1) {
					state = MASK;
				} else {
					state = BODY;
				}
			}
		} else if (state == MASK) {
			buffer[buffer_index++] = c;
			if (buffer_index == 4) {
				memcpy(mask, buffer, 4);
				buffer_index = 0;
				state = BODY;
			}
		} else if (state == BODY) {
			if (short_header.bits.MASK == 1) {
				body += c ^ mask[mask_count];
				if (++mask_count == 4) {
					mask_count = 0;
				}
			} else {
				body += c;
			}

			if (body.length() == payload) {
				result = true;
				break;
			}
		}
	}

	if (state == BODY && payload == 0) {
		result = true;
	}

//	if (result) {
//		std::cout << "payload=" << payload << " body=" << body << std::endl;
//	}
	return result;
}

bool WebSocketMessageFrame::is_final() {
	return short_header.bits.FIN == 1;
}

void WebSocketMessageFrame::send(SocketConnection* connection) {
	connection->send(&short_header.all, sizeof short_header.all);

	if (short_header.bits.PAYLOAD == 126u) {
		uint16_t payload_16 = to_endian_net((uint16_t)payload);
		connection->send(&payload_16, sizeof payload_16);
	} else if (short_header.bits.PAYLOAD == 127u) {
		uint64_t payload_64 = to_endian_net(payload);
		connection->send(&payload_64, sizeof payload_64);
	}
	if (short_header.bits.MASK == 1){
		// TODO
	}
	connection->send(body.c_str(), payload);
}

std::string* WebSocketMessageFrame::get_body() {
	return &body;
}

WebSocketMessageFrame::~WebSocketMessageFrame() {

}
