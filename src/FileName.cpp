/*
 * FileName.cpp
 *
 *  Created on: May 10, 2014
 *      Author: enrico
 */

#include "FileName.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

#include <string>
#include <fstream>
#include <iostream>

FileName::FileName(const std::string path, const std::string name)
{
	this->parent = NULL;
	this->path = path;
	if (path.length() > 1 && path[path.length() -1] == '/') {
		this->path = path.substr(0, path.length() -1);
	}

	if (name.length() > 0) {
		if (name[0] == '/') {
			this->path += name;
		} else {
			this->path += "/" + name;
		}
	}

	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
	     perror("GetCurrentDir");
	     throw std::exception();
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	current_path = std::string(cCurrentPath);

	// TODO Windows
	if (path.substr(0, 1) == "/") {
		absolute_path = this->path;
	} else {
		absolute_path = current_path + "/" + this->path;
	}
	std::string::size_type found = this->path.find_last_of("/\\");
	if (found == std::string::npos) {
		simple_name = this->path;
	} else {
		simple_name = this->path.substr(found + 1);
	}
	found = simple_name.find_last_of(".");
	if (found == std::string::npos) {
		ext = "";
	} else {
		ext = simple_name.substr(found + 1);
	}
}

std::string FileName::get_path() {
	return path;
}

std::string FileName::get_ext() {
	return ext;
}

std::string FileName::get_simple_name() {
	return simple_name;
}

FileName* FileName::get_parent() {
	std::string::size_type found = path.find_last_of("/\\");
	if (found == std::string::npos) {
		return NULL;
	} else {
		if (parent == NULL) {
			if (path == "/") {
				return NULL;
			}
			parent = new FileName(path.substr(0, found));
		}
		return parent;
	}
}

bool FileName::copy_to(const std::string& dest) {
	// TODO complete file name (from current dir)
    std::ifstream  src(absolute_path, std::ios::binary);
    if (!src.is_open()) {
    	perror(std::string("Cannot open file " + path).c_str());
    	return false;
    }
    std::ofstream dst(dest, std::ios::binary);
    if (!dst.is_open()) {
    	perror(std::string("Cannot open file " + dest).c_str());
    	return false;
    }

    dst << src.rdbuf();
    return true;
}

FileName::~FileName() {
	if (parent != NULL) {
		delete parent;
	}
}

