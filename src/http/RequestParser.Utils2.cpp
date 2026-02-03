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

/*
	Return true if c is an ASCII control character (0-31) or DEL (127).
	Control characters must never appear in the request target path.
*/
static bool	isAsciiControl(unsigned char c)
{
	if (c <= 31)
		return (true);
	if (c == 127)
		return (true);
	return (false);
}

/*
	Collapse repeated slashes:
		"/a//b///c" -> "/a/b/c"

	This avoids ambiguous routing behavior and makes location matching stable.
*/
static void	collapseDoubleSlashes(std::string& path)
{
	std::string out;
	out.reserve(path.size());

	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == '/' && !out.empty() && out[out.size() - 1] == '/')
			continue;
		out.push_back(path[i]);
	}
	path = out;
}

/*
	Reject unsafe path segments such as "." or "..".
	This prevents directory traversal and avoids ambiguous canonicalization.
*/
bool	hasUnsafeSegments(const std::string& path)
{
	size_t i = 0;

	while (i < path.size())
	{
		// skip leading slashes
		while (i < path.size() && path[i] == '/')
			++i;

		// find end of segment
		size_t j = i;
		while (j < path.size() && path[j] != '/')
			++j;

		// extract segment
		if (j > i)
		{
			std::string seg = path.substr(i, j - i);

			if (seg == "." || seg == "..")
				return (true);
		}

		i = j;
	}

	return (false);
}

/*
	Validate and normalize a parsed URL path.
	- must start with '/'
	- no ASCII control chars
	- collapse '//' to '/'
	- reject '.' and '..' segments
*/
bool	sanitizeUrlPath(std::string& path)
{
	if (path.empty())
		return (false);

	if (path[0] != '/')
		return (false);

	// Reject ASCII control characters anywhere in the path
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (isAsciiControl(static_cast<unsigned char>(path[i])))
			return (false);
	}

	// Normalize repeated slashes
	collapseDoubleSlashes(path);

	// Reject traversal-like segments
	if (hasUnsafeSegments(path))
		return (false);

	return (true);
}
