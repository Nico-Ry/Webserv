#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include "Request.hpp"

/*
	ParserState = where we currently are while parsing one HTTP request.

	The HTTP request arrives as raw bytes, often in many small chunks.
	Example:

		"GET / HTTP/1.1\r\nHo"
		"st: example.com\r\n\r\n"

	Because of that, we cannot assume we have the full request at once.
	We parse it step-by-step (state machine).
*/
enum ParserState
{
	PS_START_LINE,   // Parsing: "GET /path HTTP/1.1"
	PS_HEADERS,      // Parsing: "Host: ...", "User-Agent: ...", until empty line
	PS_BODY,         // (later) Parsing message body: Content-Length or chunked
	PS_DONE,         // Finished parsing 1 full request
	PS_ERROR         // Parsing failed, _errorStatus tells which HTTP error (e.g. 400)
};

bool	sanitizeUrlPath(std::string& path);
/*
	HttpRequestParser turns raw bytes into a HttpRequest object.

	- Module 1 (network) will read() from sockets and call feed(data).
	- This parser will store bytes in _buffer until it has complete lines.
	- When parsing is complete, isDone() becomes true and you can call getRequest().
*/
class HttpRequestParser
{
public:
	HttpRequestParser();

	// Reset the parser so we can parse a NEW request on the same connection
	// (important for keep-alive where multiple requests share 1 socket).
	void reset();

	void	resetKeepBuffer();
	bool	hasBufferedData() const;

	bool	shouldCloseConnection() const;

	/*
		feed(data)
		- Add new raw bytes to the parser.
		- The parser appends them to _buffer and tries to parse as much as possible.
		- It does NOT block. It only uses what is available.
	*/
	void feed(const std::string &data);

	// True when we successfully parsed one complete request (start line + headers + body).
	bool isDone() const;

	// True if parsing failed.
	bool hasError() const;

	// If hasError() is true, this returns the HTTP status to respond with (usually 400).
	int getErrorStatus() const;

	// Only call this after isDone() == true (and hasError() == false).
	// Returns the parsed request.
	const HttpRequest &getRequest() const;

private:
	// Current state of the parsing process (start line, headers, body, etc.)
	ParserState  _state;

	/*
		Raw input buffer:
		- We append new bytes here.
		- We remove bytes from here as we successfully parse lines/body.
		- This allows incremental parsing when data arrives in pieces.
	*/
	std::string  _buffer;

	// The request we are building while parsing
	HttpRequest  _req;

	// If state is PS_ERROR, this holds the HTTP error status code (e.g. 400)
	int          _errorStatus;

	/*
		Parsing helpers:
		These return true if they made progress (consumed input / changed state),
		false if they need more data to continue.
	*/
	bool	parseStartLine();   // parse "METHOD target HTTP/version"
	bool	parseHeaders();     // parse "Name: value" lines until empty line
	bool	parseBody(); 	    // parse message body
	bool	parseBodyChunked();  // parse chunked body

	bool	finalizeHeaders();
	bool	handleHeaderLine(const std::string &line);
	bool	parseContentLengthValue(size_t &outValue, const std::string &value);

	bool	parseChunkSizeLine();
	bool	consumeFinalChunkCRLF();
	bool	consumeChunkDataAndCRLF();


	/*
		readLine(hasLine)
		- Looks inside _buffer for "\r\n"
		- If not found: hasLine = false, returns empty string (need more data)
		- If found: extracts the line WITHOUT "\r\n", removes it from _buffer, hasLine = true
	*/
	std::string readLine(bool &hasLine);

	// Small string utilities (static = they don't need object state)
	static std::string trim(const std::string &s);       // removes spaces/tabs at both ends
	static std::string toLower(const std::string &s);    // makes header names case-insensitive

	/*
		setError(statusCode)
		- sets _state = PS_ERROR
		- sets _errorStatus = statusCode
		- returns true (so caller can treat it as "progress happened")
	*/
	bool setError(int statusCode);
};

#endif

