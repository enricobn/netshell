/*
 * HttpRequest.h
 *
 *  Created on: Apr 2, 2014
 *      Author: enrico
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <map>
#include <string>

#include "../IRequest.h"
#include "HttpRequestURI.h"

enum HttpProtocol {
	PROTOCOL_NONE, HTTP10, HTTP11
};

enum HttpMethod {
	METHOD_NONE, GET, HEAD, POST, PUT, DELETE, OPTIONS, TRACE
};

class HttpRequest : public IRequest {
private:
	enum State {
		START_LINE,
		HEADERS,
		BODY
	};

	HttpMethod method;
	HttpProtocol protocol;
	std::string path;
	HttpRequestURI* uri;
	std::map<std::string, std::string> headers;
	std::string current;
	State state;

//	void parse_request();

	void parse_start_line(std::string& line);

	void parse_header(std::string& header);

	// TODO optimize with pointers?
	/*
	 * returns true if the body is complete
	 */
	bool parse_body(std::string::iterator start, std::string::iterator end);

public:
	HttpRequest();

	const HttpMethod get_method() const;

	const std::string get_path() const;

	const HttpRequestURI* get_uri();

	const HttpProtocol get_protocol() const;

	const std::string get_header(const std::string name) const;

	virtual bool add(char* data, int len);

	virtual ~HttpRequest();
};

#endif /* HTTPREQUEST_H_ */
