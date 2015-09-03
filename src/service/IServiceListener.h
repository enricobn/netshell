/*
 * IServiceListener.h
 *
 *  Created on: May 1, 2014
 *      Author: enrico
 */

#ifndef ISERVICELISTENER_H_
#define ISERVICELISTENER_H_

#include "ServiceRequest.h"

class IServiceListener {
public:
	IServiceListener();

	virtual void on_request(ServiceRequest* request) = 0;

	virtual ~IServiceListener();
};

#endif /* ISERVICELISTENER_H_ */
