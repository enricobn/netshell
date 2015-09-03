/*
 * HttpResponse.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: enrico
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>

#include <sys/stat.h>
#include <fcntl.h>

#include "HttpResponse.h"
#include "HttpStatus.h"
#include "ContentTypeProvider.h"
#include "../utils.h"
#include "../FileName.h"

#define FILE_BUFFER_SIZE 1024
#define LOG false

const std::string HttpResponse::CONTENT_LENGTH = "CONTENT-LENGTH";
const std::string HttpResponse::CONTENT_TYPE = "CONTENT-TYPE";

void send(SocketConnection& connection, std::string s) {
	#if defined LOG
		if (LOG) {
			std::cout << s;
		}
	#endif

	connection.send(s.c_str(), s.size());
}

void send(SocketConnection& connection, HttpStatus& status, HttpProtocol protocol) {
	if (protocol == HTTP10) {
		::send(connection, "HTTP/1.0 ");
	} else if (protocol == HTTP11) {
		::send(connection, "HTTP/1.1 ");
	} else {
		// TODO
	}

	::send(connection, int_to_string(status.get_code()));
	::send(connection, " " + status.get_message());
	::send(connection, "\r\n");
}

void send_content_length(SocketConnection& connection, size_t size) {
	::send(connection, HttpResponse::CONTENT_LENGTH);
	::send(connection, ": ");

	// integer to string
	std::ostringstream ss;
	ss << size;

	::send(connection, ss.str());

	::send(connection, "\r\n");
}

HttpResponse::HttpResponse(HttpProtocol protocol) :
	status(HttpStatus::status_200_OK) ,
	headers(std::map<std::string, std::string>()),
	body(""),
	file(""),
	protocol(protocol)
{
}

void HttpResponse::set_status(HttpStatus status) {
	headers["status"] = int_to_string(status.get_code()) + " " + status.get_message();
	this->status = status;
}

void HttpResponse::send(SocketConnection* conn) {
	SocketConnection& connection = *conn;

	try {
		if (body.size() > 0) {
				::send(connection, status, protocol);
			headers["Content-type"] = "text/html";
			headers["Content-length"] = int_to_string(body.size());

			for(auto iter = headers.begin(); iter != headers.end(); ++iter) {
				::send(connection, iter->first + ": " + iter->second + "\r\n");
			}
			::send(connection, "\r\n");
			::send(connection, body);
		} else if (file.size() > 0) {
			char data_to_send[FILE_BUFFER_SIZE];
			int fd, bytes_read;

			if ( (fd = open(file.c_str(), O_RDONLY))!=-1 )    //FILE FOUND
			{
				struct stat st;
				stat(file.c_str(), &st);

				if (S_ISDIR(st.st_mode)) {
					close(fd);
					set_status(HttpStatus::status_404_Not_Found);
					// a trick to send an empty body
					file = "";
					send(conn);
				}
				headers["Content-length"] = int_to_string(st.st_size);

				#ifdef LOG_RESPONSE
					std::cout << "file size: " << st.st_size << std::endl;
				#endif

				::send(connection, status, protocol);
				for(auto iter = headers.begin(); iter != headers.end(); ++iter) {
					::send(connection, iter->first + ": " + iter->second + "\r\n");
				}
				::send(connection, "\r\n");

				while ( (bytes_read = read(fd, data_to_send, FILE_BUFFER_SIZE -1)) > 0 ) {
					connection.send(data_to_send, bytes_read);
				}
				// TODO close must be done even on exception or read < 0
				close(fd);
			} else {
				set_status(HttpStatus::status_404_Not_Found);
				// a trick to send an empty body
				file = "";
				send(conn);
			}
		} else {
			::send(connection, status, protocol);
			headers[CONTENT_LENGTH] = "0";
			// TODO
			// I put a default (application/octet-stream) content type, since browsers seems not to show an error,
			// for 0 length responses, without content type or (I think) a text content type, but I think it's my mistake ...
			headers[CONTENT_TYPE] = ContentTypeProvider::getInstance().get_type("");
			for(auto iter = headers.begin(); iter != headers.end(); ++iter) {
				::send(connection, iter->first + ": " + iter->second + "\r\n");
			}
			::send(connection, "\r\n");
		}
	} catch (std::exception &e) {
		std::cerr << "Error sending response: " << e.what() << std::endl;
		throw e;
	}
}

void HttpResponse::set_body(std::string& body) {
	if (file.size() > 0) {
		perror("file path already set");
		throw std::exception();
	}
	this->body = body;
}

void HttpResponse::set_header(std::string key, std::string value) {
	std::string upperKey = key;
	std::transform(upperKey.begin(), upperKey.end(), upperKey.begin(), ::toupper);

	if (upperKey == CONTENT_LENGTH) {
		perror("Cannot set content length.");
		throw std::exception();
	}

	headers[key] = value;
}

void HttpResponse::set_file(std::string& path) {
	FileName file_name = FileName(path);
	set_file(path, ContentTypeProvider::getInstance().get_type(file_name.get_ext()));
}

void HttpResponse::set_file(std::string& path, std::string content_type) {
	if (body.size() > 0) {
		perror("body already set");
		throw std::exception();
	}

	FileName file_name = FileName(path);

	set_header(CONTENT_TYPE, content_type);

	file = path;
}

HttpResponse::~HttpResponse() {
}

