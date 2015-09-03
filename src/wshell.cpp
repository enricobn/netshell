#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "http/HttpServer.h"
#include "http/HttpApplication.h"
#include "HttpTerminal.h"
#include "FileName.h"
#include "JSONObject.h"
#include "JSONParser.h"

#include "service/ServiceServer.h"

void print_help() {
	std::cout 	<< "Usage: wshell start" << std::endl
			 	<< "              (to start server)" << std::endl
			 	<< "       wshell download file" << std::endl
			 	<< "              (to download a file)" << std::endl
				;
}

void* start_service_server(void* service_server) {
	ServiceServer* server = (ServiceServer*) service_server;
	server->start();
	return NULL;
}

void start_server() {
	// NOT USED for now
//	ServiceServer* service_server = new ServiceServer("5001");
//	pthread_t* th = (pthread_t*)malloc(sizeof th);
//
//	int res = pthread_create(th, NULL, start_service_server, service_server);
//	if (res)
//	{
//		perror("pthread_create()");
//		throw std::exception();
//	}

	HttpServer server = HttpServer("5000");

	HttpApplication* wshell = server.create_application("wshell");
	wshell->add("terminal", new HttpTerminal());

	server.start();
}

void download(const std::string& file) {
	FileName file_name = FileName(file);

	char* tmp_file_name = tmpnam(NULL);

	JSONObject json = JSONObject();
	if (file_name.copy_to(std::string(tmp_file_name))) {
		json.add("op", "download");
		json.add("tmp_file", tmp_file_name);
		json.add("name", file_name.get_simple_name());

//		std::cout << "sending command" << std::endl;
		std::cout << "\033_" << json.to_string() << "\033\\";
	}
}

int main(int argc, char *argv[]) {
	if (argc == 2) {
		const std::string arg1 = std::string(argv[1]);
		if (arg1 == "start") {
			start_server();
			exit(0);
		}
	} else if (argc == 3) {
		const std::string arg1 = std::string(argv[1]);
		const std::string arg2 = std::string(argv[2]);
		if (arg1 == "download") {
			download(arg2);
			exit(0);
		}
	}
	print_help();
	exit(1);
}
