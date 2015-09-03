/*
 * ITerminalHandler.h
 *
 *  Created on: Apr 20, 2014
 *      Author: enrico
 */

#ifndef ITERMINALHANDLER_H_
#define ITERMINALHANDLER_H_

#include <stddef.h>

class ITerminalListener {
public:
	ITerminalListener();

	virtual void on_terminal_output(const char* buffer, size_t size) = 0;

	virtual void on_attr_change(bool icrnl) = 0;

	virtual ~ITerminalListener();
};

#endif /* ITERMINALHANDLER_H_ */
