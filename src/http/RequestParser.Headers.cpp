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

bool	HttpRequestParser::handleHeaderLine(const std::string &line)
{
	std::string::size_type	pos;
	std::string				name;
	std::string				value;

	/*
		Normal header line case:
			Name: value

		Must contain ':' or the request is malformed.
	*/
	pos = line.find(':');
	if (pos == std::string::npos)
		return (setError(400));

	/*
		Split the header into name and value.
	*/
	name = line.substr(0, pos);
	value = line.substr(pos + 1);

	/*
		Normalize:
		- Header names are case-insensitive. We store them in lowercase.
		- Trim spaces around name and value.
	*/
	name = toLower(trim(name));
	value = trim(value);

	if (name.empty() == true)
		return (setError(400));

	/*
		Store header in the request map.
		If the same header appears multiple times, this overwrites.
	*/
	_req.headers[name] = value;

	return (true);
}

bool	HttpRequestParser::parseContentLengthValue(size_t &outValue, const std::string &value)
{
	std::string			s;
	std::istringstream	iss;
	char				extra;

	/*
		We want Content-Length to be ONLY digits (no extra text).
		Examples:
			"5"     -> OK
			"  5 "  -> OK (after trim)
			"5abc"  -> ERROR
			""      -> ERROR
	*/
	s = trim(value);
	if (s.empty() == true)
		return (false);

	outValue = 0;
	iss.clear();
	iss.str(s);

	/*
		Read a number into outValue.
	*/
	if (!(iss >> outValue))
		return (false);

	/*
		If there is anything left after the number, reject it.
		This rejects "5abc".
	*/
	if (iss >> extra)
		return (false);

	return (true);
}

bool	HttpRequestParser::finalizeHeaders()
{
	/*
		END OF HEADERS

		This is where we validate rules that require knowing all headers,
		and where we decide if there is a body (Content-Length or chunked).
	*/

	/*
		RULE 1: Unknown method -> 501
	*/
	if (_req.method == METHOD_UNKNOWN)
		return (setError(501));

	/*
		RULE 2: HTTP/1.1 requires Host header
	*/
	if (_req.httpVersion == "HTTP/1.1")
	{
		if (_req.headers.find("host") == _req.headers.end())
			return (setError(400));
	}

	/*
		Reset body detection flags before reading headers that control body.
	*/
	_req.chunked = false;
	_req.hasContentLength = false;
	_req.contentLength = 0;

	/*
		RULE 3: Transfer-Encoding handling
		- If Transfer-Encoding exists and includes "chunked", we mark chunked.
		- If Transfer-Encoding exists but is NOT chunked, return 501 (unsupported).
	*/
	if (_req.headers.find("transfer-encoding") != _req.headers.end())
	{
		std::string	te;

		te = toLower(trim(_req.headers["transfer-encoding"]));
		if (te.find("chunked") != std::string::npos)
			_req.chunked = true;
		else
			return (setError(501));
	}

	/*
		RULE 4: Content-Length handling (strict parsing)
	*/
	if (_req.headers.find("content-length") != _req.headers.end())
	{
		size_t	value;

		if (parseContentLengthValue(value, _req.headers["content-length"]) == false)
			return (setError(400));
		_req.hasContentLength = true;
		_req.contentLength = value;
	}

	/*
		RULE 5: Conflict rule
		If Transfer-Encoding is chunked, Content-Length must not be present.
		(For this project we treat it as 400.)
	*/
	if (_req.chunked == true && _req.hasContentLength == true)
		return (setError(400));

	/*
		Decide next state:
		- If chunked or content-length -> parse body
		- Else -> done
	*/
	if (_req.chunked == true || _req.hasContentLength == true)
	{
		_state = PS_BODY;
		return (true);
	}

	_state = PS_DONE;
	return (true);
}


bool	HttpRequestParser::parseHeaders()
{
	bool		hasLine;
	std::string	line;

	hasLine = false;
	line = readLine(hasLine);

	/*
		If we do not have a complete line yet (no "\r\n" in _buffer),
		then we cannot continue and must wait for more data.
	*/
	if (hasLine == false)
		return (false);

	/*
		An empty line means: end of headers.
		This is the separator between headers and the optional body:
			\r\n\r\n
	*/
	if (line.empty() == true)
		return (finalizeHeaders());

	/*
		Otherwise, this is a normal header line: "Name: value"
	*/
	return (handleHeaderLine(line));
}
