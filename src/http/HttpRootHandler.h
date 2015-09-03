/*
 * HttpRoothandler.h
 *
 *  Created on: Jun 3, 2014
 *      Author: enrico
 */

#ifndef HTTPROOTHANDLER_H_
#define HTTPROOTHANDLER_H_

#include "HttpHandler.h"
#include <vector>

class HttpRootHandler : public HttpHandler {
private:
	std::vector<std::string> applications;
public:
	HttpRootHandler(std::vector<std::string> applications);

	void on_http_request(HttpRequest* request, HttpResponse* response);

	virtual ~HttpRootHandler();
};

#endif /* HTTPROOTHANDLER_H_ */
