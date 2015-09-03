/*
 * HttpRequestURI.cpp
 *
 *  Created on: May 29, 2014
 *      Author: enrico
 */

#include "HttpRequestURI.h"
#include "../utils.h"
#include <iostream>

HttpRequestURI::HttpRequestURI(std::string uri) :
scheme(""),
host(""),
port(""),
path(""),
query(""),
error_message(""),
valid(true)
{
	if (uri.length() == 0) {
		this->error_message = "invalid empty URI";
		valid = false;
		return;
	}

	enum State {
			SCHEME, HOST, PORT, PATH, QUERY
	};

	State state = SCHEME;

	std::string::iterator i = uri.begin();

	std::string current = "";

	while (true) {
		switch (state) {
			case SCHEME:
				if (i == uri.end()) {
					this->error_message = "scheme: unexpected end of URI";
					valid = false;
				} else if (*i == ':') {
					this->scheme = current;
					current = "";
				} else if (this->scheme.length() > 0 && current == "//") {
					state = HOST;
					current = "";
				} else {
					current += *i;
				}
				break;
			case HOST:
				if (i == uri.end() || *i == ':') {
					this->host = current;
					state = PORT;
					current = "";
				} else if (*i == '/') {
					this->host = current;
					state = PATH;
					current = "/";
				} else {
					current += *i;
				}
				break;
			case PORT:
				if (i == uri.end() || *i == '/') {
					this->port = current;
					state = PATH;
					current = "/";
				} else {
					current += *i;
				}
				break;
			case PATH:
				if (i == uri.end() || *i == '?') {
					this->path = current;
					std::size_t pos = this->path.find("..");
					if (pos != std::string::npos) {
						this->path = "";
						i = uri.end();
						valid = false;
						this->error_message = "invalid path";
					} else {
						state = QUERY;
						current = "";
					}
				} else {
					current += *i;
				}
				break;
			case QUERY:
				if (i == uri.end()) {
					this->query = current;
					// TODO decode all
					replace_all(this->query, "%20", " ");
				} else {
					current += *i;
				}
				break;
			default:
				break;
		}
		if (i++ == uri.end()) {
			break;
		}
	}
}

std::string HttpRequestURI::get_scheme() const {
	return scheme;
}

std::string HttpRequestURI::get_host() const {
	return host;
}

std::string HttpRequestURI::get_port() const {
	return port;
}


std::string HttpRequestURI::get_path() const {
	return path;
}

std::string HttpRequestURI::get_query() const {
	return query;
}

std::string HttpRequestURI::get_error_message() const {
	return error_message;
}

bool HttpRequestURI::is_valid() const {
	return valid;
}

HttpRequestURI::~HttpRequestURI() {
	// TODO Auto-generated destructor stub
}

