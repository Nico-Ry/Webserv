#include <iostream>
#include "http/RequestParser.hpp"

/*
	Expected: 400 because Content-Length is invalid.
*/

int	main()
{
	HttpRequestParser	parser;

	parser.feed("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5abc\r\n\r\nHello");

	std::cout << "done=" << parser.isDone() << std::endl;
	std::cout << "error=" << parser.hasError() << std::endl;

	if (parser.hasError())
	{
		std::cout << "status=" << parser.getErrorStatus() << std::endl;
	}

	return (0);
}
