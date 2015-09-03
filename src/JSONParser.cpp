/*
 * JSONParser.cpp
 *
 *  Created on: May 14, 2014
 *      Author: enrico
 */

#include "JSONParser.h"
#include "utils.h"
#include <iostream>

JSONParser::JSONParser() {
}

JSONObject JSONParser::parse(std::string data) {
	JSONObject object = JSONObject();
	enum State {
		NORMAL,
		HEADER,
		VALUE,
		STRING,
		INT
	};

	State state = NORMAL;
	std::string current = "";
	std::string header = "";
	for (auto i = data.begin(); i != data.end(); i++) {
		char c = *i;
		if (state == NORMAL) {
			if (c == '{' || c == ',') {
				state = HEADER;
			} else if (c == '}') {
				break;
			} else if (c != ' ') {
				std::cerr << "unknown char in JSON object ('" << c << "')" << std::endl;
				throw std::exception();
			}
		} else if (state == HEADER) {
			if (c == ':') {
				header = current;
				current = "";
				state = VALUE;
			} else if (c != ' ' && c != '"') {
				current += c;
			}
		} else if (state == VALUE) {
			// TODO I don't recognize " inside a string
			if (c == '"') {
				state = STRING;
			} else if (c >= '0' && c <= '9') {
				state = INT;
			} else if (c == ' ') {

			} else {
				std::cerr << "unknown value of JSON object property." << std::endl;
				throw std::exception();
			}
		} else if (state == STRING) {
			// TODO I don't recognize " inside a string
			if (c == '"') {
				object.add(header, current);
				current = "";
				header = "";
				state = NORMAL;
			} else {
				current += c;
			}
		} else if (state == INT) {
			if (c == ' ' || c == ',' || c == '}') {
				object.add(header, atoi(current.c_str()));
				current = "";
				header = "";
				state = NORMAL;
				i = i--;
			} else if (c >= '0' && c <= '9') {
				current += c;
			} else {
				std::cerr << "unknown char in JSON int ('" << c << "')" << std::endl;
				throw std::exception();
			}
		}
	}
	return object;
}

JSONParser::~JSONParser() {
}

