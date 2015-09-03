/*
 * HttpApplication.cpp
 *
 *  Created on: May 29, 2014
 *      Author: enrico
 */

#include "HttpApplication.h"
#include "../FileName.h"
#include <iostream>

HttpApplication::HttpApplication(std::string path):
path(path),
handlers(std::map<std::string,HttpHandler*>())
{
}

void HttpApplication::add(std::string path, HttpHandler* handler) {
	if (path.length() == 0 || path == "/") {
		std::cerr << "Path cannot be empty nor root. Use * if you want to serve every path." << std::endl;
		throw std::exception();
	}
	if (path == "*") {
		handler->set_path("");
	} else {
		handler->set_path(path);
	}
	handler->set_application(this);
	handlers[path] = handler;
}

HttpHandler* HttpApplication::get_handler(std::string path) {
	FileName file_name = FileName(path);
	FileName* file_name_p = &file_name;
	while (file_name_p != NULL) {
		if (handlers.find(file_name_p->get_path()) != handlers.end()) {
			return handlers[file_name_p->get_path()];
		}
		file_name_p = file_name_p->get_parent();
	}
	if (handlers.find("*") != handlers.end()) {
		return handlers["*"];
	}
	return NULL;
}

std::string HttpApplication::get_path() {
	return path;
}

HttpApplication::~HttpApplication() {
}

