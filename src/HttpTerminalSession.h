/*
 * HttpTerminalSession.h
 *
 *  Created on: May 7, 2014
 *      Author: enrico
 */

#ifndef HTTPTERMINALSESSION_H_
#define HTTPTERMINALSESSION_H_

#include "terminal/ITerminalListener.h"
#include "terminal/Terminal.h"
#include "SocketConnection.h"
#include "HttpTerminal.h"
#include "TerminalCommandReader.h"

class HttpTerminal;

class HttpTerminalSession : public ITerminalListener {
private:
//	enum State {
//		NORMAL,
//		ESC,
//		APP_COMMAND,
//		APP_COMMAND_ESC
//	};
	SocketConnection* websocket_connection;
	HttpTerminal* http_terminal;
	Terminal* terminal;
//	State state;
//	std::string response;
	TerminalCommandReader terminalOutputCommandReader;
	TerminalCommandReader userInputCommandReader;
public:
	HttpTerminalSession(HttpTerminal* http_terminal, SocketConnection* websocket_connection);

	void on_terminal_output(const char* , size_t);

	void on_attr_change(bool icrnl);

	void on_user_input(const char* buffer, size_t size);

	virtual ~HttpTerminalSession();
};

#endif /* HTTPTERMINALSESSION_H_ */
