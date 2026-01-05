#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

struct HttpResponse
{
	int statusCode;
	std::string reason; // "OK", "Not Found", etc.
	std::map<std::string, std::string> headers;
	std::string body;

	HttpResponse(int code = 200, const std::string &msg = "OK")
		: statusCode(code), reason(msg) {}
};

#endif
