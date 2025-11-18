#include "http/RequestParser.hpp"
#include <sstream>   // for std::istringstream
#include <cctype>    // for std::tolower

// --------- ctor / reset ----------

HttpRequestParser::HttpRequestParser()
: _state(PS_START_LINE), _buffer(""), _req(), _errorStatus(0) {}

void HttpRequestParser::reset() {
	_state       = PS_START_LINE;
	_buffer      = "";
	_req         = HttpRequest(); // reset to default-constructed request
	_errorStatus = 0;
}

// --------- public API ----------

void HttpRequestParser::feed(const std::string &data)
{
	if (_state == PS_ERROR || _state == PS_DONE)
		return; // ignore if already finished or errored

	_buffer.append(data);

	bool progress = true;
	while (progress) {
		progress = false;

		if (_state == PS_START_LINE) {
			progress = parseStartLine();
		} else if (_state == PS_HEADERS) {
			progress = parseHeaders();
		} else {
			// BODY handling will be done later, for now we stop
			break;
		}
	}
}

bool HttpRequestParser::isDone() const {
	return _state == PS_DONE;
}

bool HttpRequestParser::hasError() const {
	return _state == PS_ERROR;
}

int HttpRequestParser::getErrorStatus() const {
	return _errorStatus;
}

const HttpRequest &HttpRequestParser::getRequest() const {
	return _req;
}

// --------- helpers ----------

std::string HttpRequestParser::readLine(bool &hasLine) {
	hasLine = false;
	std::string::size_type pos = _buffer.find("\r\n");
	if (pos == std::string::npos)
		return "";

	std::string line = _buffer.substr(0, pos);
	_buffer.erase(0, pos + 2); // remove line + "\r\n"
	hasLine = true;
	return line;
}

std::string HttpRequestParser::trim(const std::string &s) {
	std::string::size_type start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
		++start;

	std::string::size_type end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
		--end;

	return s.substr(start, end - start);
}

std::string HttpRequestParser::toLower(const std::string &s) {
	std::string out = s;
	for (std::string::size_type i = 0; i < out.size(); ++i) {
		out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
	}
	return out;
}

bool HttpRequestParser::setError(int statusCode) {
	_state       = PS_ERROR;
	_errorStatus = statusCode;
	return true;
}

// --------- parsing logic ----------

bool HttpRequestParser::parseStartLine() {
	bool hasLine = false;
	std::string line = readLine(hasLine);
	if (!hasLine)
		return false; // need more data

	// Simple sanity limit for start-line length
	if (line.size() > 8192) {
		return setError(400); // Bad Request
	}

	std::istringstream iss(line);
	std::string methodStr;
	std::string target;
	std::string version;

	if (!(iss >> methodStr >> target >> version)) {
		return setError(400); // malformed
	}

	// Method
	if (methodStr == "GET")
		_req.method = METHOD_GET;
	else if (methodStr == "POST")
		_req.method = METHOD_POST;
	else if (methodStr == "DELETE")
		_req.method = METHOD_DELETE;
	else
		_req.method = METHOD_UNKNOWN; // later you may turn this into 501

	_req.rawTarget   = target;
	_req.httpVersion = version;

	// Very basic version check for now
	if (version != "HTTP/1.0" && version != "HTTP/1.1") {
		return setError(400);
	}

	_state = PS_HEADERS;
	return true;
}

bool HttpRequestParser::parseHeaders() {
	bool hasLine = false;
	std::string line = readLine(hasLine);
	if (!hasLine)
		return false; // need more data

	// Empty line -> end of headers
	if (line.empty()) {
		// Decide about body later.
		// For now, we pretend there is no body:
		_state = PS_DONE;
		return true;
	}

	// Header line: Name: value
	std::string::size_type pos = line.find(':');
	if (pos == std::string::npos) {
		return setError(400); // malformed header
	}

	std::string name  = line.substr(0, pos);
	std::string value = line.substr(pos + 1);

	name  = toLower(trim(name));
	value = trim(value);

	if (name.empty()) {
		return setError(400);
	}

	_req.headers[name] = value;

	// Later you'll inspect headers here (Host, Content-Length, etc.)
	return true;
}
