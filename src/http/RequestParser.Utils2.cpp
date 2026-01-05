#include "http/RequestParser.hpp"
#include <cctype>

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
	if (pos == std::string::npos)
	{
		// No full line yet
		return ("");
	}

	// Extract the line WITHOUT the "\r\n"
	std::string line = _buffer.substr(0, pos);

	// Remove the line + "\r\n" from buffer
	_buffer.erase(0, pos + 2);

	hasLine = true;
	return (line);
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

	return (s.substr(start, end - start));
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
	return (out);
}
