/*
 * SocketConnection.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: enrico
 */

#include "SocketConnection.h"

#include <sys/socket.h>
#include "utils.h"

SocketConnection::SocketConnection(int connfd) : connfd(connfd) {
}

int SocketConnection::read(void* data, size_t len) {
	return recv(connfd, data, len, 0);
}

ssize_t SocketConnection::send(const void* data, size_t len) const {
	// I haven't understood well why, but without MSG_NOSIGNAL, refreshing repeatedly the refresh button, hangs the server
	// TODO must I throw an exception if return size is < 0 ?
	return ::send(connfd, data, len, MSG_NOSIGNAL);
}

bool SocketConnection::operator <(const SocketConnection& connection) const {
	return this->connfd < connection.connfd;
}

std::string SocketConnection::to_string() {
	return int_to_string(connfd);
}

SocketConnection::~SocketConnection() {
	if (connfd >= 0) {
		shutdown (connfd, SHUT_RDWR);         //All further send and receive operations are DISABLED...
		close(connfd);
		printf("connection closed fd=%d\n", connfd);
	}
}


