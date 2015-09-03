/*
 * ServiceRequest.cpp
 *
 *  Created on: May 1, 2014
 *      Author: enrico
 */

#include "ServiceRequest.h"

# include <stddef.h>

ServiceRequest::ServiceRequest() :
state(TYPE),
type(' '),
length(0),
body(NULL),
body_length(0)
{

}

bool ServiceRequest::add(char* data, int len) {
	for (int i = 0; i < len; i++) {
		char c = data[i];
		if (state == TYPE) {
			if (c == ':') {
				state = LEGTH;
			} else {
				type = c;
			}
		} else if (state == LEGTH) {
			if (c == ':') {
				state = BODY;
				body = new char[length];
				body_length = 0;
			} else {
				length = 10 * length + (c -'0');
			}
		} else {
			// TODO optimize with a single read
			body[body_length++] = c;
			if (body_length == length) {
				return true;
			}
		}
	}
	return false;
}

char* ServiceRequest:: get_body() {
	return body;
}

ServiceRequest::~ServiceRequest() {
	if (body != NULL) {
		delete(body);
	}
}

