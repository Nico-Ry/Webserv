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

/**
 * @brief 	build(resp, closeConnection)

	This returns the full HTTP response message:

	- Status line
	- Headers
	- Blank line
	- Body
	@attention Recurring Headers such as Date, content-length, etc.
	are built in HttpResponse constructors
 */
std::string	ResponseBuilder::build(const HttpResponse &resp, bool closeConnection)
{
	std::stringstream	ss;

	/*
		1) Status line
		We always respond using HTTP/1.1 for simplicity.
	*/
	ss	<< "HTTP/1.1 "
		<< toStringSize(resp.statusCode) << " "
		<< resp.reason << CRLF;

	/*
		2) handle unique Connection Header
	*/
	if (closeConnection)
		ss << "Connection: close" << CRLF;
	else
		ss << "Connection: keep-alive" << CRLF;

	/*
		3) insert other headers defined during routing
	*/
	StringMap	h = resp.headers;
	for (StringMap::const_iterator it = h.begin(); it != h.end(); ++it)
		ss << it->first << it->second << CRLF;
	ss << CRLF; // mark end of headers

	/*
		4) insert body
	*/
	ss << resp.body;
	std::cout << resp.body << std::endl;
	return (ss.str());
}
