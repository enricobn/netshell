/*
 * JSONObject.cpp
 *
 *  Created on: May 10, 2014
 *      Author: enrico
 */

#include "JSONObject.h"
#include "utils.h"

#include <map>

JSONObject::JSONObject() :
int_map(std::map<const std::string, int>()),
string_map(std::map<const std::string, std::string>()),
object_map(std::map<const std::string, JSONObject>())
{
}

void JSONObject::add(const std::string key, int value) {
	int_map[key] = value;
}

void JSONObject::add(const std::string key, std::string value) {
	string_map[key] = value;
}

void JSONObject::add(const std::string key, JSONObject value) {
	object_map[key] = value;
}

bool JSONObject::has_key(const std::string key) {
	return int_map.find(key) != int_map.end() ||
			string_map.find(key) != string_map.end() ||
			object_map.find(key) != object_map.end();
}

int JSONObject::get_int(const std::string key) {
	return int_map[key];
}

std::string JSONObject::get_string(const std::string key) {
	return string_map[key];
}

JSONObject JSONObject::get_object(const std::string key) {
	return object_map[key];
}

std::string JSONObject::to_string() {
	std::string current = "";

	for (auto iter = int_map.begin(); iter != int_map.end(); ++iter) {
		if (current.length() > 0) {
			current += ", ";
		}
		current += "\"" + iter->first + "\":" + int_to_string(iter->second);
	}

	for (auto iter = string_map.begin(); iter != string_map.end(); ++iter) {
		if (current.length() > 0) {
			current += ", ";
		}
		current += "\"" + iter->first + "\":\"" + iter->second + "\"";
	}

	for (auto iter = object_map.begin(); iter != object_map.end(); ++iter) {
		if (current.length() > 0) {
			current += ", ";
		}
		current += "\"" + iter->first + "\":" + iter->second.to_string();
	}
	return "{" + current + "}";
}

JSONObject::~JSONObject() {
}

