/*
 * Socket.cpp
 *
 *  Created on: Mar 29, 2014
 *      Author: enrico
 */

#include <iostream>

#include <arpa/inet.h>
#include <sys/select.h>

#include <netdb.h>
#include <string.h>

#include "IRequest.h"
#include "IResponse.h"
#include "SocketServer.h"

#include <map>
#include <signal.h>
#include <string.h>

// TODO parameter?
#define REQUEST_BUFFER_SIZE 1024

/*
 * use of select function from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#select
 */
SocketServer::SocketServer(std::string port) : port(port), listenfd(-1) {
	request_buffer = (char*) malloc (REQUEST_BUFFER_SIZE);

    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port.c_str(), &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        throw std::exception();
    }

	// socket and bind
	for (p = res; p!=NULL; p=p->ai_next)
	{
		listenfd = socket (p->ai_family, p->ai_socktype, 0);
		if (listenfd == -1) {
			continue;
		} else {
			printf("socket created fd=%d\n", listenfd);
		}

		// allow socket descriptor to be reuseable
		int on = 1;
		if (setsockopt(listenfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
		{
		   perror("setsockopt() failed");
		   close(listenfd);
		   throw std::exception();
		}

		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		} else {
			close(listenfd);
		}
	}

	if (p==NULL)
	{
		perror ("socket() or bind()");
		throw std::exception();
	}

	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);

	// listen for incoming connections
	std::cout << "listening on port " << port << std::endl;

	// I listen for a maximum of 20 connections
	if ( listen (listenfd, 20) != 0 )
	{
		perror("listen() error");
		throw std::exception();
	}

	// add the listener to the master set
	FD_SET(listenfd, &master);

	// keep track of the biggest file descriptor
	fdmax = listenfd; // so far, it's this one
}

//void thread_handler(const int connfd, void (*handler)(const Connection& connection));
//

/*
 * I use it because I don't want to pass directly the connection to the thread constructor, since
 * it's copied and the destructor is called, closing the connection
 */
void thread_handler(const int connfd, void (*handler)(const SocketConnection& connection)) {
	handler(SocketConnection(connfd));
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//static void signal_handler(int sig) {
//	std::cout << "SocketServer signal received" << std::endl;
//}

void SocketServer::start() {
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];
//	sigset_t mask;
//	sigset_t orig_mask;

	// key = connection id
	std::map<int, SocketConnection*> connections;

//	/**
//	 * I'm not sure of this signal handling code, but for now it works:
//	 * I register a signal handler for the process, then in the handler I look if it's a SIGKILL for the thread, then
//	 * flag it as killed.
//	 * The sigmask code, I think, it's for handling correctly a signal while pselect is waiting, then exit from it
//	 */
//	struct sigaction act;
//
//	memset (&act, 0, sizeof(act));
//	act.sa_handler = signal_handler;
//
//	/* should shut down on SIGTERM. */
//	if (sigaction(SIGTERM, &act, 0)) {
//		perror ("sigaction");
//		throw std::exception();
//	}
//
//	sigemptyset (&mask);
//	sigaddset (&mask, SIGPIPE);
//
//	if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
//		perror ("sigprocmask");
//		throw std::exception();
//	}

	// main loop
	for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        int i;
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
            	if (i == listenfd) {
            		// handle new connection
            		addrlen = sizeof remoteaddr;
            		int connfd = accept(listenfd, (struct sockaddr *)&remoteaddr, &addrlen);

            		if (connfd == -1) {
            			perror("accept() error");
            		} else {
            			FD_SET(connfd, &master); // add to master set
						if (connfd > fdmax) {    // keep track of the max
							fdmax = connfd;
						}
						printf("new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							connfd);
            		}

            	} else {
            		// handle data from a client

            		auto found = connections.find(i);
            		if (found == connections.end()) {
            			connections[i] = new SocketConnection(i);
            		}
            		SocketConnection *connection = connections[i];


            		try {
            		    int rcvd;

            			rcvd = connection->read(request_buffer, REQUEST_BUFFER_SIZE);

            			if (rcvd < 0)    // receive error
            			{
            				perror("recv() error");
            				throw std::exception();
            			}
            			else if (rcvd == 0)
            			{
            				fprintf(stderr,"Client disconnected unexpectedly.\n");
            				throw std::exception();
            			}

						if (data_received(connection, request_buffer, rcvd)) {
							connections.erase(i);
							delete(connection);

							FD_CLR(i, &master); // remove from master set
						}
            		} catch (std::exception &e) {
            			std::cerr << "exception handling request: " << e.what() << std::endl;
            			connections.erase(i);
            			connection_closed(connection);
            			delete(connection);
            			FD_CLR(i, &master); // remove from master set
            		}
            	}
            }
        }
	}
}

SocketServer::~SocketServer() {
	if (listenfd != -1) {
		close(listenfd);
		printf("socket closed fd:%d\n", listenfd);
	}
	if (request_buffer != NULL) {
		free(request_buffer);
	}
}
