#include "http/HttpResponse.hpp"

/*
	We explicitly initialize all members (even empty ones) to avoid
	undefined values in C++98.

	- headers(): ensure the map starts in a known empty state
	- body(""): body may be filled later; start empty to be safe
	- isRedirect(false): only the redirect constructor should set this to true
	- redirectTarget(""): avoid garbage when this is not a redirect

	This guarantees predictable behavior for all HttpResponse objects.
*/


// NOTE:
// Date/Server/Content-Length are now computed in ResponseBuilder::build().
// This prevents stale Content-Length when resp.body is modified later.

HttpResponse::HttpResponse(int code, const std::string& msg)
	: statusCode(code),
	  reason(msg),
	  headers(),
	  body(""),
	  isRedirect(false),
	  redirectTarget("")
{
}

HttpResponse::HttpResponse(int code, const std::string& msg, const std::string& body)
	: statusCode(code),
	  reason(msg),
	  headers(),
	  body(body),
	  isRedirect(false),
	  redirectTarget("")
{
}

HttpResponse::HttpResponse(const std::string& redirection, int code, const std::string& msg)
	: statusCode(code),
	  reason(msg),
	  headers(),
	  body(""),
	  isRedirect(true),
	  redirectTarget(redirection)
{
	this->headers["Location"] = redirection;
}


// Utility functions


std::string	buildDateValue()
{
	std::time_t		now;
	std::tm			*gmt;
	char			buf[128];

	now = std::time(NULL);
	gmt = std::gmtime(&now);

	/*
		strftime format:
		%a = weekday short name
		%d = day (01-31)
		%b = month short name
		%Y = year
		%H:%M:%S = time
	*/
	if (gmt == NULL)
		return ("");
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (std::string(buf));
}

std::string	toStringSize(size_t n)
{
	std::ostringstream	oss;

	oss << n;
	return (oss.str());
}


// HttpResponse::HttpResponse(int code, const std::string& msg)
// 	: statusCode(code), reason(msg) {
// 		this->headers["Date: "] = buildDateValue();
// 		this->headers["Server: "] = "webserv";
// 		this->headers["Content-Length: "] = toStringSize(this->body.size());
// 	}


// HttpResponse::HttpResponse(int code, const std::string& msg, const std::string& body)
// 	: statusCode(code), reason(msg), body(body) {
// 		this->headers["Date: "] = buildDateValue();
// 		this->headers["Server: "] = "webserv";
// 		this->headers["Content-Length: "] = toStringSize(this->body.size());
// 	}


// HttpResponse::HttpResponse(const std::string& redirection, int code, const std::string& msg)
// 	: statusCode(code), reason(msg), isRedirect(true), redirectTarget(redirection) {
// 		this->headers["Date: "] = buildDateValue();
// 		this->headers["Server: "] = "webserv";
// 		this->headers["Content-Length: "] = toStringSize(this->body.size());
// 		this->headers["Location: "] = redirection;
// 	}



