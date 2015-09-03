/*
 * HttpRoothandler.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: enrico
 */

#include "HttpRootHandler.h"

HttpRootHandler::HttpRootHandler(std::vector<std::string> applications) :
applications(applications)
{
}

void HttpRootHandler::on_http_request(HttpRequest* request, HttpResponse* response) {
	std::string body = "<html><body>";
	body += "shttp version 1.0.0<br/>";
	body += "<br/>";
	body += "Registered applications:<br/>";
	for (auto it = applications.begin(); it != applications.end(); it++) {
		body += "<a href=\"" + *it + "\"/>" + *it + "</br>";
	}
	body += "</body></html>";
	response->set_body(body);
}

HttpRootHandler::~HttpRootHandler() {
}

