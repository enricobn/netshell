/*
 * ContentTypeProvider.cpp
 *
 *  Created on: May 12, 2014
 *      Author: enrico
 */

#include "ContentTypeProvider.h"

#include <map>
#include <string>

ContentTypeProvider::ContentTypeProvider() :
content_types(std::map<std::string, std::string>())
{
	// TODO content types
	content_types["html"] = "text/html";
	content_types["js"] = "application/x-javascript";
}

std::string ContentTypeProvider::get_type(std::string ext) {
	if (content_types.find(ext) == content_types.end()) {
		return "application/octet-stream";
	}
	return content_types[ext];
}

