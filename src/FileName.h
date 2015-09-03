/*
 * FileName.h
 *
 *  Created on: May 10, 2014
 *      Author: enrico
 */

#ifndef FILENAME_H_
#define FILENAME_H_

#include <string>

class FileName {
private:
	std::string path;
	std::string current_path;
	std::string absolute_path;
	std::string simple_name;
	std::string ext;
	FileName* parent;
public:
	FileName(const std::string path, const std::string name = "");

	bool copy_to(const std::string& dest);

	std::string get_path();

	/**
	 * name of file without path
	 */
	std::string get_simple_name();

	std::string get_ext();

	FileName* get_parent();

	virtual ~FileName();
};

#endif /* FILENAME_H_ */
