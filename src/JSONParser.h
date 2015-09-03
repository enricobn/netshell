/*
 * JSONParser.h
 *
 *  Created on: May 14, 2014
 *      Author: enrico
 */

#ifndef JSONPARSER_H_
#define JSONPARSER_H_

#include <string>
#include "JSONObject.h"

class JSONParser {
public:
	JSONParser();

	JSONObject parse(std::string data);

	virtual ~JSONParser();
};

#endif /* JSONPARSER_H_ */
