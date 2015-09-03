/*
 * HttpResponse.h
 *
 *  Created on: Apr 7, 2014
 *      Author: enrico
 */

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include <string>
#include <map>

#include "HttpStatus.h"
#include "HttpRequest.h"
#include "../IResponse.h"
#include "../SocketConnection.h"

class HttpResponse : public IResponse {
private:
	HttpStatus status;
	std::map<std::string, std::string> headers;
	std::string body;
	// TODO it can be binary
	std::string file;
	HttpProtocol protocol;
public:
	static const std::string CONTENT_LENGTH;
	static const std::string CONTENT_TYPE;

	HttpResponse(HttpProtocol protocol);

	virtual void send(SocketConnection* connection);

	void set_status(HttpStatus status);

	void set_header(std::string key, std::string value);

	/**
	 * an helper method to send the content of file
	 */
	void set_file(std::string& path, std::string content_type);
	/**
	 * an helper method to send the content of file, the content type is taken from MIME of path
	 */
	void set_file(std::string& path);

	void set_body(std::string& body);

	virtual ~HttpResponse();
};

#endif /* HTTPRESPONSE_H_ */
