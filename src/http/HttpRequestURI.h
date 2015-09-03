/*
 * HttpRequestURI.h
 *
 *  Created on: May 29, 2014
 *      Author: enrico
 */

#ifndef HTTPREQUESTURI_H_
#define HTTPREQUESTURI_H_

#include <string>

class HttpRequestURI {
private:
	std::string scheme;
	std::string host;
	std::string port;
	std::string path;
	std::string query;
	std::string error_message;
	bool valid;;
public:
	HttpRequestURI(std::string uri);

	std::string get_scheme() const;

	std::string get_host() const;

	std::string get_port() const;

	std::string get_path() const;

	std::string get_query() const;

	std::string get_error_message() const;

	bool is_valid() const;

	virtual ~HttpRequestURI();
};

#endif /* HTTPREQUESTURI_H_ */
