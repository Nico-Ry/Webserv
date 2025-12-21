#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

enum HttpMethod {
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
	METHOD_UNKNOWN
};

struct HttpRequest {
	HttpMethod method;

	std::string rawTarget; // "/path?x=1"
	std::string path;      // "/path"
	std::string query;     // "x=1"

	std::string httpVersion; // "HTTP/1.1"

	std::map<std::string, std::string> headers;

	// Body metadata
	bool hasContentLength;
	size_t contentLength;
	bool chunked;
	size_t bodyBytesRead;

	/* Chunked decoding state (used only when chunked == true) */
	size_t	chunkSize;			// current chunk size we are reading
	bool	chunkSizeKnown;		// true after we parsed the chunk size line


	// Body buffer (optional for now)
	std::string body;

	HttpRequest()
		: method(METHOD_UNKNOWN),
		  hasContentLength(false),
		  contentLength(0),
		  chunked(false),
		  bodyBytesRead(0),
		  chunkSize(0),
		  chunkSizeKnown(false)

	{}
};

#endif
