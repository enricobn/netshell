/*
 * IMessage.h
 *
 *  Created on: Apr 5, 2014
 *      Author: enrico
 */

#ifndef IREQUEST_H_
#define IREQUEST_H_

class IRequest {
public:
	/**
	 * returns true if the message is complete
	 * TODO I think for a more general purpose it must be void*
	 */
	virtual bool add(char* data, int len) = 0;

	virtual ~IRequest() = 0;
};

#endif /* IREQUEST_H_ */
