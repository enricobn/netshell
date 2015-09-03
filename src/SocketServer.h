/*
 * Socket.h
 *
 *  Created on: Mar 29, 2014
 *      Author: enrico
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>

#include "IRequest.h"
#include "IResponse.h"
#include "SocketConnection.h"

class SocketServer {
	private:
		const std::string port;
		int listenfd;
		fd_set master;    // master file descriptor list
		fd_set read_fds;  // temp file descriptor list for select()
		int fdmax;        // maximum file descriptor number
		char* request_buffer;

	public:
		SocketServer(std::string port);

		void start();

		virtual bool data_received(SocketConnection* connection, char* data, int len) = 0;

		virtual void connection_closed(SocketConnection* connection) = 0;

		virtual ~SocketServer();
};

#endif /* SOCKET_H_ */
