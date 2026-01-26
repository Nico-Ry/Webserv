#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

typedef std::map<std::string, std::string> StringMap;

struct HttpResponse
{
	int			statusCode;
	std::string reason; // "OK", "Not Found", etc.
	StringMap	headers;
	std::string body;
	bool		isRedirect;
	std::string	redirectTarget;

	// HttpResponse(int code = 200, const std::string &msg = "OK")
	// 	: statusCode(code), reason(msg) {}
	HttpResponse(int code, const std::string& msg)
		: statusCode(code), reason(msg) {}
	HttpResponse(int code, const std::string& msg, const std::string& body)
		: statusCode(code), reason(msg), body(body) {}
	HttpResponse(const std::string& redirection, int code, const std::string& msg)
		: statusCode(code), reason(msg), isRedirect(true), redirectTarget(redirection) {}
	bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
};

#endif
