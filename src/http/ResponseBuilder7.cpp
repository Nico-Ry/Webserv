#include "http/ResponseBuilder.hpp"
#include <sstream>
#include <ctime>

/*
	toStringSize(n)

	C++98 does not have std::to_string.
	We use stringstream instead.
*/
std::string	ResponseBuilder::toStringSize(size_t n)
{
	std::ostringstream	oss;

	oss << n;
	return (oss.str());
}

/*
	buildDateValue()

	Build a Date header value.
	We use GMT (UTC) time because HTTP Date uses GMT.

	Format example:
		Sun, 21 Dec 2025 09:00:00 GMT
*/
std::string	ResponseBuilder::buildDateValue()
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

/*
	build(resp, closeConnection)

	This returns the full HTTP response message:

	- Status line
	- Headers
	- Blank line
	- Body
*/
std::string	ResponseBuilder::build(const HttpResponse &resp, bool closeConnection)
{
	std::string											out;
	std::map<std::string, std::string>::const_iterator	it;
	bool												hasContentLength;

	/*
		1) Status line
		We always respond using HTTP/1.1 for simplicity.
	*/
	out = "HTTP/1.1 ";
	out += toStringSize(resp.statusCode);
	out += " ";
	out += resp.reason;
	out += "\r\n";

	/*
		2) Standard headers (Date + Server)
	*/
	out += "Date: ";
	out += buildDateValue();
	out += "\r\n";
	out += "Server: webserv\r\n";
	/*
		3) Connection header (based on closeConnection parameter)
	*/
	out += "Connection: ";
	if (closeConnection == true)
		out += "close\r\n";
	else
		out += "keep-alive\r\n";

	/*
		4) Content-Length header
		We ensure it exists even if user did not set it.
	*/
	hasContentLength = false;
	it = resp.headers.find("Content-Length");
	if (it != resp.headers.end())
		hasContentLength = true;

	if (hasContentLength == false)
	{
		out += "Content-Length: ";
		out += toStringSize(resp.body.size());
		out += "\r\n";
	}

	/*
		5) Add all user-provided headers from resp.headers
		We do not try to normalize case; Module 3 can choose.
	*/
	it = resp.headers.begin();
	while (it != resp.headers.end())
	{
		/*
			Do not duplicate Content-Length if we already added it above.
			If the user provided it, we keep their version.
		*/
		if (it->first == "Content-Length" && hasContentLength == false)
		{
			++it;
			continue ;
		}

		out += it->first;
		out += ": ";
		out += it->second;
		out += "\r\n";
		++it;
	}

	/*
		6) End headers
	*/
	out += "\r\n";

	/*
		7) Append body
	*/
	out += resp.body;

	return (out);
}
