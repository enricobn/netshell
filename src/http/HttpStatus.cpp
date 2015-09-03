/*
 * HttpStatus.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: enrico
 */
#include <utility>

#include "HttpStatus.h"

const HttpStatus HttpStatus::status_100_Continue = HttpStatus(100, "Continue");
const HttpStatus HttpStatus::status_101_Switching_Protocols = HttpStatus(101, "Switching Protocols");
const HttpStatus HttpStatus::status_200_OK = HttpStatus(200, "OK");
const HttpStatus HttpStatus::status_201_Created = HttpStatus(201, "Created");
const HttpStatus HttpStatus::status_202_Accepted = HttpStatus(202, "Accepted");
const HttpStatus HttpStatus::status_203_Non_Authoritative_Information = HttpStatus(203, "Non-Authoritative Information");
const HttpStatus HttpStatus::status_204_No_Content = HttpStatus(204, "No Content");
const HttpStatus HttpStatus::status_205_Reset_Content = HttpStatus(205, "Reset Content");
const HttpStatus HttpStatus::status_206_Partial_Content = HttpStatus(206, "Partial Content");
const HttpStatus HttpStatus::status_300_Multiple_Choices = HttpStatus(300, "Multiple Choices");
const HttpStatus HttpStatus::status_301_Moved_Permanently = HttpStatus(301, "Moved Permanently");
const HttpStatus HttpStatus::status_302_Found = HttpStatus(302, "Found");
const HttpStatus HttpStatus::status_303_See_Other = HttpStatus(303, "See Other");
const HttpStatus HttpStatus::status_304_Not_Modified = HttpStatus(304, "Not Modified");
const HttpStatus HttpStatus::status_305_Use_Proxy = HttpStatus(305, "Use Proxy");
const HttpStatus HttpStatus::status_307_Temporary_Redirect = HttpStatus(307, "Temporary Redirect");
const HttpStatus HttpStatus::status_400_Bad_Request = HttpStatus(400, "Bad Request");
const HttpStatus HttpStatus::status_401_Unauthorized = HttpStatus(401, "Unauthorized");
const HttpStatus HttpStatus::status_402_Payment_Required = HttpStatus(402, "Payment Required");
const HttpStatus HttpStatus::status_403_Forbidden = HttpStatus(403, "Forbidden");
const HttpStatus HttpStatus::status_404_Not_Found = HttpStatus(404, "Not Found");
const HttpStatus HttpStatus::status_405_Method_Not_Allowed = HttpStatus(405, "Method Not Allowed");
const HttpStatus HttpStatus::status_406_Not_Acceptable = HttpStatus(406, "Not Acceptable");
const HttpStatus HttpStatus::status_407_Proxy_Authentication_Required = HttpStatus(407, "Proxy Authentication Required");
const HttpStatus HttpStatus::status_408_Request_Time_out = HttpStatus(408, "Request Time-out");
const HttpStatus HttpStatus::status_409_Conflict = HttpStatus(409, "Conflict");
const HttpStatus HttpStatus::status_410_Gone = HttpStatus(410, "Gone");
const HttpStatus HttpStatus::status_411_Length_Required = HttpStatus(411, "Length Required");
const HttpStatus HttpStatus::status_412_Precondition_Failed = HttpStatus(412, "Precondition Failed");
const HttpStatus HttpStatus::status_413_Request_Entity_Too_Large = HttpStatus(413, "Request Entity Too Large");
const HttpStatus HttpStatus::status_414_Request_URI_Too_Large = HttpStatus(414, "Request-URI Too Large");
const HttpStatus HttpStatus::status_415_Unsupported_Media_Type = HttpStatus(415, "Unsupported Media Type");
const HttpStatus HttpStatus::status_416_Requested_range_not_satisfiable = HttpStatus(416, "Requested range not satisfiable");
const HttpStatus HttpStatus::status_417_Expectation_Failed = HttpStatus(417, "Expectation Failed");
const HttpStatus HttpStatus::status_500_Internal_Server_Error = HttpStatus(500, "Internal Server Error");
const HttpStatus HttpStatus::status_501_Not_Implemented = HttpStatus(501, "Not Implemented");
const HttpStatus HttpStatus::status_502_Bad_Gateway = HttpStatus(502, "Bad_Gateway");
const HttpStatus HttpStatus::status_503_Service_Unavailable = HttpStatus(503, "Service Unavailable");
const HttpStatus HttpStatus::status_504_Gateway_Time_out = HttpStatus(504, "Gateway Time-out");
const HttpStatus HttpStatus::status_505_HTTP_Version_not_supported = HttpStatus(505, "HTTP Version not supported");

HttpStatus::HttpStatus( int code, std::string message ) : code(code), message(message) {
}

const int HttpStatus::get_code() const {
	return code;
}

const std::string HttpStatus::get_message() const {
	return message;
}

HttpStatus::~HttpStatus() {
}

