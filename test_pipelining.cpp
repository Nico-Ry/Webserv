#include <iostream>
#include "http/RequestParser.hpp"

/*
	Test program for Phase 3.7 (multiple requests in the same buffer)

	We feed TWO complete requests at once.
	The parser should:
	- parse the first
	- keep remaining bytes in _buffer
	- after resetKeepBuffer(), parse the second WITHOUT receiving new data
*/

static void	print_request(const HttpRequest &req)
{
	std::cout << "Parsed request:" << std::endl;
	std::cout << "	method (enum) = " << req.method << std::endl;
	std::cout << "	target = " << req.rawTarget << std::endl;
	std::cout << "	version = " << req.httpVersion << std::endl;
	if (req.headers.find("host") != req.headers.end())
	{
		std::cout << "	host = " << req.headers.find("host")->second << std::endl;
	}
}

int	main()
{
	HttpRequestParser	parser;

	std::string	all;

	all =
		"GET /first HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"\r\n"
		"GET /second HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"\r\n";

	/*
		Feed both requests at once.
	*/
	parser.feed(all);

	std::cout << "After first feed:" << std::endl;
	std::cout << "	isDone = " << parser.isDone() << std::endl;
	std::cout << "	hasError = " << parser.hasError() << std::endl;
	std::cout << "	hasBufferedData = " << parser.hasBufferedData() << std::endl;

	if (parser.isDone() && !parser.hasError())
	{
		print_request(parser.getRequest());
	}
	else
	{
		std::cout << "First request failed." << std::endl;
		return (0);
	}

	/*
		Now parse the second request that is already in _buffer.
	*/
	parser.resetKeepBuffer();
	parser.feed("");

	std::cout << "After parsing second request from buffer:" << std::endl;
	std::cout << "	isDone = " << parser.isDone() << std::endl;
	std::cout << "	hasError = " << parser.hasError() << std::endl;
	std::cout << "	hasBufferedData = " << parser.hasBufferedData() << std::endl;

	if (parser.isDone() && !parser.hasError())
	{
		print_request(parser.getRequest());
	}
	else
	{
		std::cout << "Second request failed." << std::endl;
	}

	return (0);
}
