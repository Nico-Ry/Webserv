#include "http/RequestParser.hpp"
#include <cctype>
#include "utils.hpp"

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
		return ;

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
			progress = parseStartLine();
		else if (_state == PS_HEADERS)
			progress = parseHeaders();
		else if (_state == PS_BODY)
			progress = parseBody();
		else
			break ;
	}
	// [DEBUG]
	std::cout << GOLD << _state << RES << std::endl;
	printHttpRequest(_req);
}


/*
	isDone()

	Returns true when the full request has been parsed successfully.
	This function is marked const because it does NOT modify the object.
*/
bool HttpRequestParser::isDone() const
{
	return (_state == PS_DONE);
}

/*
	hasError()

	Returns true if parsing failed.
*/
bool HttpRequestParser::hasError() const
{
	return (_state == PS_ERROR);
}
