/*
 * HttpServer.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: enrico
 */

#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "WebSocketMessage.h"
#include "../FileName.h"
#include "../SocketConnection.h"
#include "HttpRootHandler.h"

#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

const std::string to_sh1_base64(const std::string& in) {
	const unsigned char* str = reinterpret_cast<const unsigned char*>(in.c_str());
	unsigned char hash[SHA_DIGEST_LENGTH]; // == 20

	SHA1(str, in.size(), hash);

	BIO *b64 = BIO_new(BIO_f_base64());

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

	BIO *mem = BIO_new(BIO_s_mem()); // create BIO that holds the result

	BIO_push(b64, mem);

	BIO_write(b64, hash, SHA_DIGEST_LENGTH);

	if (!BIO_flush(b64))
	{
		perror("BIO_flush error");
		throw new std::exception();
	}

	// get a pointer to mem's data
	char* output;
	int output_len = BIO_get_mem_data(mem, &output);

	std::string result = std::string(output, 0, output_len);

	BIO_free_all(b64);

	return result;
}

//const HttpStaticHandler HttpServer::static_handler = HttpStaticHandler();

HttpServer::HttpServer(std::string port) :
	SocketServer(port),
	requests(std::map<SocketConnection*, IRequest*>()),
	applications(std::map<std::string, HttpApplication*>()),
	web_sockets(std::map<SocketConnection*, IRequest*>())
{
}

IRequest* HttpServer::create_web_socket_request() {
	return new WebSocketMessage();
}

IRequest* HttpServer::create_request() {
	return new HttpRequest();
}

IResponse* HttpServer::create_response(IRequest* request) {
	HttpRequest* req = (HttpRequest*) request;
	return new HttpResponse(req->get_protocol());
}

HttpHandler* HttpServer::get_handler(HttpRequest* request) {
	if (request->get_uri()->get_path().length() == 0 || request->get_uri()->get_path() == "/") {
		std::vector<std::string> v;
		for(auto it = applications.begin(); it != applications.end(); ++it) {
		  v.push_back(it->first);
		}
		// TODO memory leak
		return new HttpRootHandler(v);
	}

	HttpApplication* application = NULL;

	#ifdef LOG_REQUEST
		std::cout << "HttpServer request->get_uri()->get_path()=" << request->get_uri()->get_path() << std::endl;
	#endif

	FileName file_name = FileName(request->get_uri()->get_path().substr(1));
	FileName* file_name_p = &file_name;
	while (file_name_p != NULL) {
		if (applications.find(file_name_p->get_path()) != applications.end()) {
			application = applications[file_name_p->get_path()];
			break;
		}
		file_name_p = file_name_p->get_parent();
	}

	if (application == NULL) {
		return NULL;
	}

	std::string app_relative_path;
	// it's the application's root
	if (file_name.get_path() == file_name_p->get_path()) {
		app_relative_path = "";
	} else {
		app_relative_path = file_name.get_path().substr(file_name_p->get_path().length() + 1);
	}
	HttpHandler* handler = application->get_handler(app_relative_path);
	if (handler == NULL) {
		// TODO memory leak
		handler = new HttpStaticHandler(file_name_p->get_path());
	}
	return handler;
}

HttpApplication* HttpServer::create_application(std::string path) {
	HttpApplication* application = new HttpApplication(path);
	applications[path] = application;
	return application;
}

bool HttpServer::data_received(SocketConnection* connection, char* data, int len) {
	IRequest* request;
	auto request_found = requests.find(connection);
	bool web_socket = false;
	// TODO I don't know how it can be NULL, but happens ...
	if (request_found == requests.end() || request_found->second == NULL) {
		auto web_socket_found = web_sockets.find(connection);
		if (web_socket_found != web_sockets.end()) {
			 request = create_web_socket_request();
			web_socket = true;
		} else {
			request = create_request();
		}
		requests[connection] = request;
	} else {
		request = request_found->second;
	}

	bool result = false;
	if (request->add(data, len)) {
		// TODO use a thread
//							std::thread th = std::thread(thread_handler, connfd, handler);
//
//							th.detach();
		if (web_socket) {
			auto web_socket_found = web_sockets.find(connection);
			if (web_socket_found == web_sockets.end()) {
				// TODO
				std::cerr << "Cannot find web_socket" << std::endl;
			} else {
				HttpRequest* http_request = (HttpRequest*)web_socket_found->second;

				HttpHandler* handler = get_handler(http_request);
				if (handler != NULL) {
					WebSocketMessage* ws_request = (WebSocketMessage*) request;
					if (ws_request->is_close()) {
						handler->on_websocket_close(http_request, connection);
						web_sockets.erase(connection);
						// this closes the connection
						result = true;
					} else {
						handler->on_websocket_request(http_request, connection, ws_request->get_body());
					}
				} else {
					// TODO
					std::cerr << "Cannot find websocket handler" << std::endl;
				}
			}

			requests.erase(connection);
			delete(request);
		} else {
			HttpRequest* http_request = (HttpRequest*)request;

			// TODO template?
			HttpResponse* response = (HttpResponse*)create_response(http_request);

			// TODO Upgrade can be one of Connection values
			std::string connection_header = http_request->get_header("Connection");
			std::size_t s = connection_header.find("Upgrade");
			if (http_request->get_uri()->is_valid() &&
					s != std::string::npos &&
					http_request->get_header("Upgrade") == "websocket")
			{
				std::cout << "received a websocket open request" << std::endl;

				response->set_status(HttpStatus::status_101_Switching_Protocols);
				std::string key = http_request->get_header("Sec-WebSocket-Key"); //"dGhlIHNhbXBsZSBub25jZQ==";//http_request->get_header("Sec-WebSocket-Key");

				key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

				response->set_header("Sec-WebSocket-Accept", to_sh1_base64(key));

//				std::cout << to_sh1_base64("dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11") << std::endl;

				response->set_header("Connection", "Upgrade");
				response->set_header("Upgrade", "websocket");

				HttpHandler* handler = get_handler(http_request);
				try {
					response->send(connection);

					web_sockets[connection] = request;

					if (handler != NULL) {
						handler->on_websocket_connect(http_request, connection);
					} else {
						//TODO
					}
				} catch(std::exception &e) {
					std::cout << "Error sending websocket accept on connection " << connection->to_string()
							<< ": " << e.what() << std::endl;
					web_sockets.erase(connection);
					handler->on_websocket_close(http_request, connection);
					result = true;
				}
				delete(response);

				requests.erase(connection);

//				delete(request);

			} else {
				if (http_request->get_uri()->is_valid()) {
					HttpHandler* handler = get_handler(http_request);
					if (handler != NULL) {
						handler->on_http_request(http_request, response);
					} else {
						response->set_status(HttpStatus::status_404_Not_Found);
					}
				} else {
					response->set_status(HttpStatus::status_400_Bad_Request);
				}

				std::string conn_header = http_request->get_header("CONNECTION");
				if (http_request->get_protocol() == HTTP10) {
					if (conn_header != "keep-alive") {
						result = true;
					} else {
						response->set_header("Connection", "keep-alive");
					}
				} else {
					if (conn_header == "close") {
						result = true;
					}
				}

				try {
					response->send(connection);
				} catch(std::exception &e) {
					std::cout << "Error sending response on connection " << e.what() << std::endl;
					result = true;
				}

				delete(response);

				requests.erase(connection);

				delete(request);

			}

		}
	}
	return result;
}

void HttpServer::connection_closed(SocketConnection* connection) {
	IRequest* request = requests[connection];
	if (request != NULL) {
		auto web_socket_found = web_sockets.find(connection);
		if (web_socket_found != web_sockets.end()) {
			HttpRequest* http_request = (HttpRequest*)web_socket_found->second;
			web_sockets.erase(connection);
			HttpHandler* handler = get_handler(http_request);
			if (handler != NULL) {
				handler->on_websocket_close(http_request, connection);
			}
		}
		requests.erase(connection);

		delete(request);
	}
}

HttpServer::~HttpServer() {
	for (auto handler = applications.begin(); handler != applications.end(); handler++) {
		delete(handler->second);
	}

	for (auto websocket = web_sockets.begin(); websocket != web_sockets.end(); websocket++) {
		delete(websocket->first);
		delete(websocket->second);
	}

	for (auto request = requests.begin(); request != requests.end(); request++) {
		delete(request->first);
		delete(request->second);
	}

}

