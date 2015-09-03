/*
 * HttpApplication.h
 *
 *  Created on: May 29, 2014
 *      Author: enrico
 */

#ifndef HTTPAPPLICATION_H_
#define HTTPAPPLICATION_H_

#include <string>
#include <map>
#include "HttpHandler.h"

class HttpHandler;

class HttpApplication {
private:
	std::string path;
	// key = path
	std::map<std::string, HttpHandler*> handlers;
public:
	HttpApplication(std::string path);

	std::string get_path();

	void add(std::string path, HttpHandler* handler);

	/**
	 * the path is relative to the application's path
	 */
	HttpHandler* get_handler(std::string path);

	virtual ~HttpApplication();
};

#endif /* HTTPAPPLICATION_H_ */
