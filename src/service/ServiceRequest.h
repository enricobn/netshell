/*
 * ServiceRequest.h
 *
 *  Created on: May 1, 2014
 *      Author: enrico
 */

#ifndef SERVICEREQUEST_H_
#define SERVICEREQUEST_H_

// a request is made of type:length:body
class ServiceRequest {
private:
	enum State {
		TYPE,
		LEGTH,
		BODY
	};
	State state;
	char type;
	int length;
	char* body;
	int body_length;
public:
	ServiceRequest();

	bool add(char* data, int len);

	char* get_body();

	virtual ~ServiceRequest();
};

#endif /* SERVICEREQUEST_H_ */
