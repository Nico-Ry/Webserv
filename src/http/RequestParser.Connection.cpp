#include "http/RequestParser.hpp"
#include <cctype>

/*
	getErrorStatus()

	If hasError() is true, this tells the caller which HTTP status
	should be returned (usually 400 Bad Request).
*/
int HttpRequestParser::getErrorStatus() const
{
	return (_errorStatus);
}

/*
	getRequest()

	Gives access to the parsed HttpRequest.
	Only valid if isDone() == true and hasError() == false.
*/
const HttpRequest &HttpRequestParser::getRequest() const
{
	return (_req);
}

bool	HttpRequestParser::hasBufferedData() const
{
	/*
		Returns true if there are still unconsumed bytes in the internal buffer.
		This is useful to know if another request might already be waiting.
	*/
	if (_buffer.empty() == true)
		return (false);
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
			return (true);
		if (_req.httpVersion == "HTTP/1.0" && value.find("keep-alive") != std::string::npos)
			return (false);
	}

	/*
		Default behavior based on HTTP version
	*/
	if (_req.httpVersion == "HTTP/1.0")
		return (true);

	/*
		Default for HTTP/1.1 is keep-alive.
	*/
	return (false);
}
