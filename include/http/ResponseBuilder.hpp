#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <string>
#include "Response.hpp"

/*
	ResponseBuilder builds a raw HTTP response string from a HttpResponse struct.

	Module 3 will create HttpResponse (status, headers, body).
	Module 2 builds the final HTTP message string.
	Module 1 sends it through the socket.
*/
class ResponseBuilder
{
public:
	/*
		build(resp, closeConnection)

		- resp: status code, reason, headers, body
		- closeConnection:
			true  -> "Connection: close"
			false -> "Connection: keep-alive"
	*/
	static std::string	build(const HttpResponse &resp, bool closeConnection);

private:
	/*
		Returns a formatted Date header value.
		Example: "Sun, 21 Dec 2025 09:00:00 GMT"
	*/
	static std::string	buildDateValue();

	/*
		Convert size_t to string without std::to_string (not available in C++98).
	*/
	static std::string	toStringSize(size_t n);
};

#endif
