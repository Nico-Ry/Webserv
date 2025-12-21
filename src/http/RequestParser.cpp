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
	CONSTRUCTOR

	This is called automatically when you write:

		HttpRequestParser parser;

	The part after ':' is called an "initializer list".
	It initializes the member variables BEFORE the constructor body runs.
*/
HttpRequestParser::HttpRequestParser()
	: _state(PS_START_LINE),   // We start by parsing the start line
	  _buffer(""),             // No data received yet
	  _req(),                  // Default-constructed HttpRequest
	  _errorStatus(0)          // No error
{
	// Constructor body is empty because everything is initialized above
}

/*
	reset()

	This prepares the parser to parse a NEW request.
	This is important for HTTP keep-alive, where the same connection
	can send multiple requests one after another.
*/
void HttpRequestParser::reset()
{
	_state = PS_START_LINE;
	_buffer.clear();           // remove any leftover raw data
	_req = HttpRequest();      // reset request to default values
	_errorStatus = 0;
}

/*
	feed(data)

	This function is called by Module 1 (network code).
	It receives raw bytes read from the socket.

	IMPORTANT:
	- Data may be incomplete
	- Data may contain more than one request (later)
*/
void	HttpRequestParser::feed(const std::string &data)
{
	bool	progress;
	// If parsing is already finished or failed, ignore new data
	if (_state == PS_DONE || _state == PS_ERROR)
		return;

	// Append new data to our internal buffer
	_buffer.append(data);

	/*
		We try to parse as much as possible.
		parseStartLine() and parseHeaders() will return:
			true  -> something was parsed
			false -> need more data
	*/
	progress = true;
	while (progress == true)
	{
		progress = false;
		if (_state == PS_START_LINE)
		{
			progress = parseStartLine();
		}
		else if (_state == PS_HEADERS)
		{
			progress = parseHeaders();
		}
		else if (_state == PS_BODY)
		{
			progress = parseBody();
		}
		else
		{
			break ;
		}
	}
}

bool	HttpRequestParser::parseBody()
{
	/*
		We only implement Content-Length body parsing in this step.

		At this point:
		- Headers are already parsed.
		- parseHeaders() decided there IS a body, and set _state = PS_BODY.
		- _req.hasContentLength / _req.contentLength is already set.

		We need to collect exactly _req.contentLength bytes from _buffer.
		When done, we set PS_DONE.
	*/

	/*
		If request is chunked, we are NOT implementing it yet.
		We will do chunked in Phase 3.5.
	*/
	if (_req.chunked == true)
		return (parseBodyChunked());

	/*
		If there is no Content-Length information, then something is wrong:
		PS_BODY should only happen when we know how much to read.
	*/
	if (_req.hasContentLength == false)
	{
		return (setError(400));
	}

	/*
		If Content-Length is 0, body is empty and we are done immediately.
	*/
	if (_req.contentLength == 0)
	{
		_state = PS_DONE;
		return (true);
	}

	/*
		We may not have all body bytes yet.
		We only take what's available in _buffer and wait for more if needed.
	*/
	{
		size_t	remaining;
		size_t	canTake;

		remaining = _req.contentLength - _req.body.size();

		if (_buffer.size() == 0)
		{
			return (false);
		}

		if (_buffer.size() < remaining)
		{
			canTake = _buffer.size();
		}
		else
		{
			canTake = remaining;
		}

		/*
			Append body bytes to _req.body.
		*/
		_req.body.append(_buffer, 0, canTake);

		/*
			Remove consumed bytes from _buffer.
		*/
		_buffer.erase(0, canTake);
	}

	/*
		If we collected the full body, we're done.
	*/
	if (_req.body.size() == _req.contentLength)
	{
		_state = PS_DONE;
		return (true);
	}

	/*
		Otherwise, we need more body bytes from feed().
	*/
	return (false);
}


/*
	isDone()

	Returns true when the full request has been parsed successfully.
	This function is marked const because it does NOT modify the object.
*/
bool HttpRequestParser::isDone() const
{
	return _state == PS_DONE;
}

/*
	hasError()

	Returns true if parsing failed.
*/
bool HttpRequestParser::hasError() const
{
	return _state == PS_ERROR;
}

/*
	getErrorStatus()

	If hasError() is true, this tells the caller which HTTP status
	should be returned (usually 400 Bad Request).
*/
int HttpRequestParser::getErrorStatus() const
{
	return _errorStatus;
}

/*
	getRequest()

	Gives access to the parsed HttpRequest.
	Only valid if isDone() == true and hasError() == false.
*/
const HttpRequest &HttpRequestParser::getRequest() const
{
	return _req;
}

/*
	readLine(hasLine)

	This function tries to read ONE line ending in "\r\n" from _buffer.

	Example:
		_buffer = "GET / HTTP/1.1\r\nHost: a\r\n"

	First call:
		returns "GET / HTTP/1.1"
		_buffer becomes "Host: a\r\n"
*/
std::string HttpRequestParser::readLine(bool &hasLine)
{
	hasLine = false;

	// Look for CRLF sequence
	std::string::size_type pos = _buffer.find("\r\n");
	if (pos == std::string::npos) {
		// No full line yet
		return "";
	}

	// Extract the line WITHOUT the "\r\n"
	std::string line = _buffer.substr(0, pos);

	// Remove the line + "\r\n" from buffer
	_buffer.erase(0, pos + 2);

	hasLine = true;
	return line;
}

/*
	trim(s)

	Removes whitespace from the beginning and end of a string.
	Used for header names and values.

	Example:
		"  Host  " -> "Host"
*/
std::string HttpRequestParser::trim(const std::string &s)
{
	std::string::size_type start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
		++start;

	std::string::size_type end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
		--end;

	return s.substr(start, end - start);
}

/*
	toLower(s)

	Converts a string to lowercase.
	HTTP header names are case-insensitive, so we normalize them.

	Example:
		"Host" -> "host"
*/
std::string HttpRequestParser::toLower(const std::string &s)
{
	std::string out = s;
	for (std::string::size_type i = 0; i < out.size(); ++i)
	{
		out[i] = static_cast<char>(
			std::tolower(static_cast<unsigned char>(out[i])));
	}
	return out;
}

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
	{
		return (false);
	}

	/*
		Safety limit: avoid extremely long start lines.
		(Exact limits can be adjusted later.)
	*/
	if (line.size() > 8192)
	{
		return (setError(400));
	}

	/*
		Split the start line by spaces using istringstream:

			METHOD TARGET VERSION
	*/
	iss.clear();
	iss.str(line);

	if (!(iss >> methodStr >> target >> version))
	{
		return (setError(400));
	}

	/*
		Convert METHOD string to our enum.
	*/
	if (methodStr == "GET")
	{
		_req.method = METHOD_GET;
	}
	else if (methodStr == "POST")
	{
		_req.method = METHOD_POST;
	}
	else if (methodStr == "DELETE")
	{
		_req.method = METHOD_DELETE;
	}
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
	{
		return (setError(400));
	}

	/*
		Start line is done, next we parse headers.
	*/
	_state = PS_HEADERS;

	return (true);
}

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
	{
		return (setError(400));
	}

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
	{
		return (setError(400));
	}

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
	{
		return (false);
	}

	outValue = 0;
	iss.clear();
	iss.str(s);

	/*
		Read a number into outValue.
	*/
	if (!(iss >> outValue))
	{
		return (false);
	}

	/*
		If there is anything left after the number, reject it.
		This rejects "5abc".
	*/
	if (iss >> extra)
	{
		return (false);
	}

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
	{
		return (setError(501));
	}

	/*
		RULE 2: HTTP/1.1 requires Host header
	*/
	if (_req.httpVersion == "HTTP/1.1")
	{
		if (_req.headers.find("host") == _req.headers.end())
		{
			return (setError(400));
		}
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
		{
			_req.chunked = true;
		}
		else
		{
			return (setError(501));
		}
	}

	/*
		RULE 4: Content-Length handling (strict parsing)
	*/
	if (_req.headers.find("content-length") != _req.headers.end())
	{
		size_t	value;

		if (parseContentLengthValue(value, _req.headers["content-length"]) == false)
		{
			return (setError(400));
		}
		_req.hasContentLength = true;
		_req.contentLength = value;
	}

	/*
		RULE 5: Conflict rule
		If Transfer-Encoding is chunked, Content-Length must not be present.
		(For this project we treat it as 400.)
	*/
	if (_req.chunked == true && _req.hasContentLength == true)
	{
		return (setError(400));
	}

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
	{
		return (false);
	}

	/*
		An empty line means: end of headers.
		This is the separator between headers and the optional body:
			\r\n\r\n
	*/
	if (line.empty() == true)
	{
		return (finalizeHeaders());
	}

	/*
		Otherwise, this is a normal header line: "Name: value"
	*/
	return (handleHeaderLine(line));
}


bool	HttpRequestParser::parseBodyChunked()
{
	/*
		Chunked body format (simplified):

			<hex_size>\r\n
			<data bytes...>\r\n
			<hex_size>\r\n
			<data bytes...>\r\n
			0\r\n
			\r\n

		We must decode the chunks and append the data into _req.body.
	*/

	while (true)
	{
		/*
			STEP A: If we don't know the next chunk size, we must parse the size line.
			The size line ends with "\r\n".
		*/
		if (_req.chunkSizeKnown == false)
		{
			std::string			line;
			bool				hasLine;
			std::istringstream	iss;
			size_t				sizeValue;

			hasLine = false;
			line = readLine(hasLine);

			/*
				If we don't have a full line yet, wait for more data.
			*/
			if (hasLine == false)
			{
				return (false);
			}

			/*
				Chunk size is hex (base 16). Example: "4" or "1A".
				There may be optional chunk extensions like: "4;ext=1"
				We ignore extensions by cutting at ';' if present.
			*/
			{
				std::string::size_type	pos;

				pos = line.find(';');
				if (pos != std::string::npos)
				{
					line = line.substr(0, pos);
				}
			}

			line = trim(line);

			if (line.empty() == true)
			{
				return (setError(400));
			}

			/*
				Parse hex number.
				std::istringstream + std::hex lets us read hex into an integer.
			*/
			sizeValue = 0;
			iss.clear();
			iss.str(line);
			iss >> std::hex >> sizeValue;

			if (iss.fail())
			{
				return (setError(400));
			}

			_req.chunkSize = sizeValue;
			_req.chunkSizeKnown = true;

			/*
				If chunk size is 0, that means: end of body.
				After 0\r\n there is a final "\r\n" (and optional trailer headers).
				For this project step, we just require the final "\r\n".
			*/
			if (_req.chunkSize == 0)
			{
				/*
					We must now consume the final CRLF after the 0-size chunk.
					That is: "\r\n"
				*/
				if (_buffer.size() < 2)
				{
					return (false);
				}
				if (_buffer.substr(0, 2) != "\r\n")
				{
					return (setError(400));
				}
				_buffer.erase(0, 2);

				_state = PS_DONE;
				return (true);
			}
		}

		/*
			STEP B: We know the chunk size. We must read exactly that many bytes from _buffer.
		*/
		if (_buffer.size() < _req.chunkSize)
		{
			/*
				Not enough data yet to read the full chunk.
			*/
			return (false);
		}

		/*
			Append chunk data to the decoded body.
		*/
		_req.body.append(_buffer, 0, _req.chunkSize);
		_buffer.erase(0, _req.chunkSize);

		/*
			STEP C: After chunk data, the protocol requires "\r\n".
		*/
		if (_buffer.size() < 2)
		{
			return (false);
		}
		if (_buffer.substr(0, 2) != "\r\n")
		{
			return (setError(400));
		}
		_buffer.erase(0, 2);

		/*
			Chunk finished. Next chunk size line must be parsed.
		*/
		_req.chunkSize = 0;
		_req.chunkSizeKnown = false;

		/*
			Loop again: parse next size line, read next chunk, etc.
		*/
	}
}

void	HttpRequestParser::resetKeepBuffer()
{
	/*
		Prepare to parse a NEW request,
		but DO NOT clear _buffer.

		This is important when:
		- we received 2 requests in the same socket read
		- or we received extra bytes after finishing the body
	*/
	_state = PS_START_LINE;
	_req = HttpRequest();
	_errorStatus = 0;
}

bool	HttpRequestParser::hasBufferedData() const
{
	/*
		Returns true if there are still unconsumed bytes in the internal buffer.
		This is useful to know if another request might already be waiting.
	*/
	if (_buffer.empty() == true)
	{
		return (false);
	}
	return (true);
}

bool	HttpRequestParser::shouldCloseConnection() const
{
	/*
		This function answers:
			"After handling this request, should we close the TCP connection?"

		Based on HTTP rules:

		HTTP/1.1:
			- keep-alive by default
			- close only if "Connection: close"

		HTTP/1.0:
			- close by default
			- keep alive only if "Connection: keep-alive"

		We store header names in lowercase, so we check "connection".
		We also lowercase the header value to compare safely.
	*/

	std::map<std::string, std::string>::const_iterator	it;
	std::string											value;

	it = _req.headers.find("connection");
	if (it != _req.headers.end())
	{
		value = toLower(trim(it->second));

		/*
			If there are multiple tokens like:
				"keep-alive, upgrade"
			we just check if "close" or "keep-alive" appears anywhere.
		*/
		if (value.find("close") != std::string::npos)
		{
			return (true);
		}
		if (_req.httpVersion == "HTTP/1.0" && value.find("keep-alive") != std::string::npos)
		{
			return (false);
		}
	}

	/*
		Default behavior based on HTTP version
	*/
	if (_req.httpVersion == "HTTP/1.0")
	{
		return (true);
	}

	/*
		Default for HTTP/1.1 is keep-alive.
	*/
	return (false);
}
