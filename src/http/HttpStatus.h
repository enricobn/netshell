/*
 * HttpStatus.h
 *
 *  Created on: Apr 10, 2014
 *      Author: enrico
 */

#ifndef HTTPSTATUS_H_
#define HTTPSTATUS_H_

#include <string>

class HttpStatus {
private:
	int code;
	std::string message;
public:
	static const HttpStatus status_100_Continue;
	static const HttpStatus status_101_Switching_Protocols;
	static const HttpStatus status_200_OK;
	static const HttpStatus status_201_Created;
	static const HttpStatus status_202_Accepted;
	static const HttpStatus status_203_Non_Authoritative_Information; // Non-Authoritative_Information
	static const HttpStatus status_204_No_Content;
	static const HttpStatus status_205_Reset_Content;
	static const HttpStatus status_206_Partial_Content;
	static const HttpStatus status_300_Multiple_Choices;
	static const HttpStatus status_301_Moved_Permanently;
	static const HttpStatus status_302_Found;
	static const HttpStatus status_303_See_Other;
	static const HttpStatus status_304_Not_Modified;
	static const HttpStatus status_305_Use_Proxy;
	static const HttpStatus status_307_Temporary_Redirect;
	static const HttpStatus status_400_Bad_Request;
	static const HttpStatus status_401_Unauthorized;
	static const HttpStatus status_402_Payment_Required;
	static const HttpStatus status_403_Forbidden;
	static const HttpStatus status_404_Not_Found;
	static const HttpStatus status_405_Method_Not_Allowed;
	static const HttpStatus status_406_Not_Acceptable;
	static const HttpStatus status_407_Proxy_Authentication_Required;
	static const HttpStatus status_408_Request_Time_out; // 408_Request_Time-out
	static const HttpStatus status_409_Conflict;
	static const HttpStatus status_410_Gone;
	static const HttpStatus status_411_Length_Required;
	static const HttpStatus status_412_Precondition_Failed;
	static const HttpStatus status_413_Request_Entity_Too_Large;
	static const HttpStatus status_414_Request_URI_Too_Large; // Request-URI_Too_Large
	static const HttpStatus status_415_Unsupported_Media_Type;
	static const HttpStatus status_416_Requested_range_not_satisfiable;
	static const HttpStatus status_417_Expectation_Failed;
	static const HttpStatus status_500_Internal_Server_Error;
	static const HttpStatus status_501_Not_Implemented;
	static const HttpStatus status_502_Bad_Gateway;
	static const HttpStatus status_503_Service_Unavailable;
	static const HttpStatus status_504_Gateway_Time_out; // Gateway_Time-out
	static const HttpStatus status_505_HTTP_Version_not_supported;

	HttpStatus( int code, std::string message );

	const int get_code() const;

	const std::string get_message() const;

	virtual ~HttpStatus();
};

#endif /* HTTPSTATUS_H_ */
