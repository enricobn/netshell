/*
 * Terminal1.h
 *
 *  Created on: Apr 19, 2014
 *      Author: enrico
 */

#ifndef TERMINAL1_H_
#define TERMINAL1_H_

#include "ITerminalListener.h"
#include <stddef.h>
#include <thread>

//typedef void (*ReadHandler)(const void* , unsigned long int);

class Terminal {
private:
	int fdm;
	int fds;
	// TODO make it a vector?
	ITerminalListener* terminal_listener;
	pthread_t* th;
	pid_t child_process;
	const char *file;
	const char **arg;

	static void* thread_wrapper(void* terminal);

	void run();
public:
	Terminal(ITerminalListener* terminal_listener, const char *file, size_t n_args, ...);

	void start();

	void write(const void* buffer, size_t size);

	void remove_listener();

	virtual ~Terminal();
};

#endif /* TERMINAL1_H_ */
