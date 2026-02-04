#include <iostream>
#include "http/RequestParser.hpp"

/*
	This test verifies HTTP/1.1 Host validation.
	Expected: hasError=1 and errorStatus=400
*/

int	main()
{
	HttpRequestParser	parser;

	parser.feed("GET / HTTP/1.1\r\nUser-Agent: curl/8.0\r\n\r\n");

	std::cout << "done=" << parser.isDone() << std::endl;
	std::cout << "error=" << parser.hasError() << std::endl;

	if (parser.hasError())
	{
		std::cout << "status=" << parser.getErrorStatus() << std::endl;
	}

	return (0);
}
