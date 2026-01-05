#include "http/RequestParser.hpp"
#include <cctype>

/*
	<sstream> is used for std::istringstream.
	We use it to split the start line like this:

		"GET /index.html HTTP/1.1"

	into:
		method = "GET"
		target = "/index.html"
		version = "HTTP/1.1"
*/
#include <sstream>

/*
	setError(statusCode)

	Called when the request is malformed.
	Sets the parser to PS_ERROR and stores the HTTP error code.
*/
bool	HttpRequestParser::setError(int statusCode)
{
	_state = PS_ERROR;
	_errorStatus = statusCode;
	return (true);
}

/*
	parseStartLine()

	Parses the first line of the HTTP request:

		METHOD SP TARGET SP VERSION

	Example:
		"GET /index.html HTTP/1.1"
*/
bool	HttpRequestParser::parseStartLine()
{
	bool				hasLine;
	std::string			line;
	std::istringstream	iss;
	std::string			methodStr;
	std::string			target;
	std::string			version;

	/*
		We try to read the first line of the request:

			"GET /path HTTP/1.1\r\n"

		If we don't have "\r\n" yet, readLine() will fail and we must wait.
	*/
	hasLine = false;
	line = readLine(hasLine);

	if (hasLine == false)
		return (false);

	/*
		Safety limit: avoid extremely long start lines.
		(Exact limits can be adjusted later.)
	*/
	if (line.size() > 8192)
		return (setError(400));

	/*
		Split the start line by spaces using istringstream:

			METHOD TARGET VERSION
	*/
	iss.clear();
	iss.str(line);

	if (!(iss >> methodStr >> target >> version))
		return (setError(400));

	/*
		Convert METHOD string to our enum.
	*/
	if (methodStr == "GET")
		_req.method = METHOD_GET;
	else if (methodStr == "POST")
		_req.method = METHOD_POST;
	else if (methodStr == "DELETE")
		_req.method = METHOD_DELETE;
	else
	{
		/*
			For now, we store METHOD_UNKNOWN.
			Later, you may decide to return 501 Not Implemented.
		*/
		_req.method = METHOD_UNKNOWN;
	}

	/*
		Store target and version.
		target example: "/index.html" or "/path?x=1"
		version example: "HTTP/1.1"
	*/
	_req.rawTarget = target;
	_req.httpVersion = version;

	/*
		Minimal validation for HTTP version.
	*/
	if (version != "HTTP/1.0" && version != "HTTP/1.1")
		return (setError(400));

	/*
		Start line is done, next we parse headers.
	*/
	_state = PS_HEADERS;
	return (true);
}
