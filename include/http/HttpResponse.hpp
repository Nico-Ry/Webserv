#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <sstream>
#include <ctime>

typedef std::map<std::string, std::string> StringMap;

#define CRLF "\r\n"

struct HttpResponse
{
	int					statusCode;
	std::string 		reason; // "OK", "Not Found", etc.
	StringMap			headers;
	std::string 		body;
	bool				isRedirect;
	std::string			redirectTarget;

	// HttpResponse(int code = 200, const std::string &msg = "OK")
	// 	: statusCode(code), reason(msg) {}
	HttpResponse(int code, const std::string& msg);
	HttpResponse(int code, const std::string& msg, const std::string& body);
	HttpResponse(const std::string& redirection, int code, const std::string& msg);
	bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
};

std::string	buildDateValue();
std::string	toStringSize(size_t n);

#endif
