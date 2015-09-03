/*
 * utils.h
 *
 *  Created on: Apr 5, 2014
 *      Author: enrico
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <limits.h>

#include <algorithm>
#include <string>
#include <sstream>

// from http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}


static inline std::string int_to_string(int i) {
	std::ostringstream ss;
	ss << i;

	return ss.str();
}

/**
 * converts a native number to endian net
 */
template<typename T> T to_endian_net(T native_number) {
	T result = 0;
	int i;

	for (i = (int)sizeof(result) - 1; i >= 0; i--) {
	    ((unsigned char *)&result)[i] = native_number & UCHAR_MAX;
	    native_number >>= CHAR_BIT;
	}
	return result;
}

/**
 * converts a an endian net number to native
 */
template<typename T> T to_endian_native(T net_number) {
	T result = 0;
	int i;

	for (i = 0; i < (int)sizeof(result); i++) {
		result <<= CHAR_BIT;
		result += (((unsigned char *)&net_number)[i] & UCHAR_MAX);
	}
	return result;
}

static inline void replace_all(std::string &src, std::string what, std::string with) {
	size_t found;
	while (( found = src.find(what)) != std::string::npos ) {
		src.replace (found, what.size(), with);
	}
}

#endif /* UTILS_H_ */
