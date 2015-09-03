/*
 * SocketConnection.h
 *
 *  Created on: Apr 7, 2014
 *      Author: enrico
 */

#ifndef SOCKETCONNECTION_H_
#define SOCKETCONNECTION_H_

#include <string>

class SocketConnection {
	private:
		const int connfd;

	public:
		SocketConnection(int connfd);

		ssize_t send(const void* data, size_t len) const;

		int read(void* data, size_t len);

		bool operator <(const SocketConnection& connection) const;

		std::string to_string();

		virtual ~SocketConnection();

};


#endif /* SOCKETCONNECTION_H_ */
