/*
 * HttpRequest.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: enrico
 */

#include <iostream>

#include "HttpRequest.h"
#include "../SocketConnection.h"
#include "../utils.h"
#include "../FileName.h"

#include <map>

HttpMethod parse_method(std::string method) {
    if ( method == "GET")
    {
    	return GET;
    }
    else if (method == "HEAD" )
    {
    	return HEAD;
    }
    else if ( method == "POST")
    {
    	return POST;
    }
    else if (method == "PUT")
    {
    	return PUT;
    }
    else if ( method == "DELETE" )
    {
    	return DELETE;
    }
    else if ( method == "OPTIONS" )
    {
    	return OPTIONS;
    }
    else if ( method == "TRACE" )
    {
    	return TRACE;
    }
    else
    {
    	std::cerr << "Unknown Http method " << method << std::endl;
    	throw new std::exception();
    }
}

HttpProtocol parse_protocol(std::string protocol) {
	if (protocol == "HTTP/1.0") {
		return HTTP10;
	}
	else if (protocol == "HTTP/1.1")
	{
		return HTTP11;
	}
	else
	{
    	std::cerr << "Unknown Http protocol " << protocol << std::endl;
    	throw new std::exception();

	}
}

HttpRequest::HttpRequest() :
		method(METHOD_NONE),
		protocol(PROTOCOL_NONE),
		uri(NULL),
		headers(std::map<std::string, std::string>()),
		current("") ,
		state(START_LINE)
{
}

bool HttpRequest::add(char* data, int len) {
	// TODO optimize, directly iterate over data
	std::string chunk = std::string(data, 0, len);

	if (state == BODY) {
		return parse_body(chunk.begin(), chunk.end());
	}

	std::string::iterator i = chunk.begin();

	while (i != chunk.end()) {
		// TODO I hope \r and \n cannot appear in other way than together (\r\n) to mark the end of a line
		if (*i == '\r') {
			i++;
			continue;
		}
		if (*i == '\n') {
			switch (state) {
				case START_LINE:
					parse_start_line(current);
					current.clear();
					state = HEADERS;
					break;
				case HEADERS:
					// empty line
					if (current.length() == 0) {
						state = BODY;
						return parse_body(i + 1, chunk.end());
					} else {
						parse_header(current);
						current.clear();
					}
					break;
				case BODY:
					// I cannot be here
					throw std::exception();
				default:
					break;
			}
		} else {
			current += *i;
		}
		i++;
	}
	return false;
}

void HttpRequest::parse_start_line(std::string& line) {
	enum State {
		METHOD,
		PATH,
		PROTOCOL
	};

	State state = METHOD;

	auto i = line.begin();
	auto start = i;

	bool exit = false;

	while (!exit) {
		if (i == line.end() || *i == '\t' || *i == ' ') {
			if (state == METHOD)
			{
				method = parse_method(std::string(start, i));
				state = PATH;
			}
			else if (state == PATH)
			{
				path = std::string(start, i);
				state = PROTOCOL;
			}
			else
			{
				protocol = parse_protocol(std::string(start, i));
				break;
			}
			if (i == line.end()) {
				exit = true;
			} else {
				i++;
				while (i != line.end() && (*i == '\t' || *i == ' ')) {
					i++;
				}
				start = i;
			}
		} else {
			i++;
		}
	}

}

// TODO
void HttpRequest::parse_header(std::string& header) {
//	std::cout << header << std::endl;

	// TODO handle multi-line header
	std::size_t found = header.find(':');
	if (found != std::string::npos) {
		std::string key = std::string(header, 0, found);

		std::transform(key.begin(), key.end(), key.begin(), ::toupper);

		std::string value = std::string(header, found + 1);

		trim(value);

		headers[key] = value;
	}
}


bool HttpRequest::parse_body(std::string::iterator start, std::string::iterator end) {
	auto found = headers.find("CONTENT-LENGTH");
	// if I don't find content length, there's no body I can handle for now
	// TODO handle other types of body
	if (found == headers.end()) {
		return true;
	}
	// TODO add to body then check for content-length
	return true;
}


const HttpMethod HttpRequest::get_method() const {
	return method;
}

const std::string HttpRequest::get_path() const {
	return path;
}

const HttpRequestURI* HttpRequest::get_uri() {
	if (uri == NULL) {
		#ifdef LOG_REQUEST
			std::cout << "HttpRequest path=" << path << std::endl;
			std::cout << "            Host=" << get_header("Host") << std::endl;
			std::cout << "            Referer=" << get_header("Referer") << std::endl;
		#endif

		std::string s_uri = "";

		if (get_header("Referer") != "") {
			s_uri = FileName(get_header("Referer"), path).get_path();
		} else if (get_header("Host") != "") {
			s_uri = FileName("http://" + get_header("Host"), path).get_path();
		} else {
			s_uri = FileName(path).get_path();
		}

		#ifdef LOG_REQUEST
			std::cout << "            s_uri=" << s_uri << std::endl;
		#endif

		uri = new HttpRequestURI(s_uri);

	}
	return uri;
}

const HttpProtocol HttpRequest::get_protocol() const {
	return protocol;
}

const std::string HttpRequest::get_header(const std::string name) const {
	std::string upper_name = name;
	std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

	auto found = headers.find(upper_name);
	if (found == headers.end()) {
		return "";
	}
	return (*found).second;
}


HttpRequest::~HttpRequest() {
	if (uri != NULL) {
		delete uri;
	}
}

