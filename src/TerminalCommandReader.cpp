/*
 * TerminalCommandReader.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: enrico
 */

#include "TerminalCommandReader.h"
#include "JSONObject.h"
#include "JSONParser.h"
#include <iostream>

TerminalCommandReader::TerminalCommandReader() :
state(NORMAL),
current(""),
commands()
{
}

void TerminalCommandReader::on_data_read(const char* buffer, size_t size) {
	for (unsigned long int  i = 0; i < size; i++) {
		unsigned char c = buffer[i];
		if (state == NORMAL) {
			if (c == 27) {
				state = ESC;
			}
		} else if (state == ESC) {
			if (c == '_') {
				state = APP_COMMAND;
			} else {
				state = NORMAL;
			}
		} else if (state == APP_COMMAND) {
			if (c == 27) {
				state = APP_COMMAND_ESC;
			}
		} else if (state == APP_COMMAND_ESC) {
			if (c == '\\') {
				std::string command = current.substr(2, current.length() -3);

				std::cout << "received command \"" << command << "\"" << std::endl;

				try {
					JSONParser parser = JSONParser();
					JSONObject object = parser.parse(command);
					commands.push(object);
				} catch (std::exception &e) {
					std::cerr << "Error parsing command: " << command << std::endl;
				}
				current = "";
				state = NORMAL;
				continue;
			}
		} else {
			throw std::exception();
		}

		// TODO I don't handle UTF-8 multichar sequences, but it seems to work, but I think it's system dependent
		if (c > 127) {
//			std::cout << "Unknown char:" << (int)c << std::endl;
			current += c;
		} else {
			current += c;
		}
	}

}

std::queue<JSONObject>* TerminalCommandReader::get_commands() {
	return &commands;
}

std::string TerminalCommandReader::pop_current() {
	std::string result = current;
	current = "";
	return result;
}

TerminalCommandReader::~TerminalCommandReader() {
}

