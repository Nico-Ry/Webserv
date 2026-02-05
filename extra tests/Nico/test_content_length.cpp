#include <iostream>
#include "http/RequestParser.hpp"

/*
	Test program for Phase 3.4 (Content-Length body parsing)

	We send a POST request with Content-Length: 5
	Body is: "Hello"
*/

int	main()
{
	HttpRequestParser	parser;

	/*
		Split in 3 parts to simulate socket fragmentation:
		- start line + some headers
		- end headers + partial body
		- remaining body
	*/
	parser.feed("POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nHe");

	std::cout << "After part1:" << std::endl;
	std::cout << "	isDone = " << parser.isDone() << std::endl;
	std::cout << "	hasError = " << parser.hasError() << std::endl;

	parser.feed("llo");

	std::cout << "After part2:" << std::endl;
	std::cout << "	isDone = " << parser.isDone() << std::endl;
	std::cout << "	hasError = " << parser.hasError() << std::endl;

	if (parser.isDone() && !parser.hasError())
	{
		const HttpRequest	&req = parser.getRequest();

		std::cout << "Parsed request:" << std::endl;
		std::cout << "	method (enum) = " << req.method << std::endl;
		std::cout << "	target = " << req.rawTarget << std::endl;
		std::cout << "	version = " << req.httpVersion << std::endl;
		std::cout << "	body size = " << req.body.size() << std::endl;
		std::cout << "	body = [" << req.body << "]" << std::endl;
	}
	else if (parser.hasError())
	{
		std::cout << "Parser error, HTTP status = "
				  << parser.getErrorStatus() << std::endl;
	}

	return (0);
}
