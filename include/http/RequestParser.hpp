#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include "Request.hpp"

enum ParserState {
	PS_START_LINE,
	PS_HEADERS,
	PS_BODY,     // will be used later
	PS_DONE,
	PS_ERROR
};

class HttpRequestParser {
public:
	HttpRequestParser();

	void reset();

	// Add new data (from socket) to the parser
	void feed(const std::string &data);

	bool isDone() const;
	bool hasError() const;
	int  getErrorStatus() const;       // HTTP status to use on error (400 etc.)

	const HttpRequest &getRequest() const;

private:
	ParserState  _state;
	std::string  _buffer;              // raw data not yet processed
	HttpRequest  _req;
	int          _errorStatus;         // if PS_ERROR, which HTTP status

	// Helpers
	bool parseStartLine();             // returns true if something changed
	bool parseHeaders();               // returns true if something changed

	std::string readLine(bool &hasLine);
	static std::string trim(const std::string &s);
	static std::string toLower(const std::string &s);

	bool setError(int statusCode);
};

#endif
