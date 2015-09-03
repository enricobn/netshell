/*
 * JSONObject.h
 *
 *  Created on: May 10, 2014
 *      Author: enrico
 */

#ifndef JSONOBJECT_H_
#define JSONOBJECT_H_

#include <string>
#include <map>

class JSONObject {
private:
	std::map<const std::string,int> int_map;
	std::map<const std::string, std::string> string_map;
	std::map<const std::string, JSONObject> object_map;
public:
	JSONObject();

	void add(const std::string key, int value);

	void add(const std::string key, std::string value);

	void add(const std::string key, JSONObject value);

	bool has_key(const std::string key);

	int get_int(const std::string key);

	std::string get_string(const std::string key);

	JSONObject get_object(const std::string key);

	std::string to_string();

	virtual ~JSONObject();
};

#endif /* JSONOBJECT_H_ */
