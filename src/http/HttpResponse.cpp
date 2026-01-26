#include "http/HttpResponse.hpp"

HttpResponse::HttpResponse(int code, const std::string& msg)
	: statusCode(code), reason(msg) {
		this->headers["Date: "] = buildDateValue();
		this->headers["Server: "] = "webserv";
		this->headers["Content-Length: "] = toStringSize(this->body.size());
	}


HttpResponse::HttpResponse(int code, const std::string& msg, const std::string& body)
	: statusCode(code), reason(msg), body(body) {
		this->headers["Date: "] = buildDateValue();
		this->headers["Server: "] = "webserv";
		this->headers["Content-Length: "] = toStringSize(this->body.size());
	}


HttpResponse::HttpResponse(const std::string& redirection, int code, const std::string& msg)
	: statusCode(code), reason(msg), isRedirect(true), redirectTarget(redirection) {
		this->headers["Date: "] = buildDateValue();
		this->headers["Server: "] = "webserv";
		this->headers["Content-Length: "] = toStringSize(this->body.size());
		this->headers["Location: "] = redirection;
	}




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