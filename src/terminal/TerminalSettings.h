/*
 * TerminalAttributes.h
 *
 *  Created on: Apr 30, 2014
 *      Author: enrico
 */

#ifndef TERMINALATTRIBUTES_H_
#define TERMINALATTRIBUTES_H_

#include <termios.h>

class TerminalSettings {
	private:
		struct termios term_settings;
	public:
		bool icrnl;
		bool igncr;
		bool icanon;
public:
	TerminalSettings(int file_descriptor, bool print);
	virtual ~TerminalSettings();
};

#endif /* TERMINALATTRIBUTES_H_ */
