/*
 * TerminalCommandReader.h
 *
 *  Created on: Jun 5, 2014
 *      Author: enrico
 */

#ifndef TERMINALCOMMANDREADER_H_
#define TERMINALCOMMANDREADER_H_

#include <string>
#include <queue>
#include "JSONObject.h"

class TerminalCommandReader {
private:
	enum State {
		NORMAL,
		ESC,
		APP_COMMAND,
		APP_COMMAND_ESC
	};
	State state;
	std::string current;
	std::queue<JSONObject> commands;

public:
	TerminalCommandReader();

	void on_data_read(const char* buffer, size_t size);

	std::queue<JSONObject>* get_commands();

	std::string pop_current();

	virtual ~TerminalCommandReader();
};

#endif /* TERMINALCOMMANDREADER_H_ */
