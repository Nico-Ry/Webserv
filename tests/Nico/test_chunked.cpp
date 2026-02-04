#include <iostream>
#include "http/RequestParser.hpp"

/*
	Test program for Phase 3.5 (chunked request body decoding)

	Body:
		4\r\nWiki\r\n
		5\r\npedia\r\n
		0\r\n\r\n

	Decoded:
		"Wikipedia"
*/

int	main()
{
	HttpRequestParser	parser;

	parser.feed("POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n");
	parser.feed("4\r\nWi");
	parser.feed("ki\r\n5\r\npedia\r\n0\r\n\r\n");

	std::cout << "done=" << parser.isDone() << std::endl;
	std::cout << "error=" << parser.hasError() << std::endl;

	if (parser.isDone() && !parser.hasError())
	{
		const HttpRequest	&req = parser.getRequest();

		std::cout << "decoded body size=" << req.body.size() << std::endl;
		std::cout << "decoded body=[" << req.body << "]" << std::endl;
	}
	else if (parser.hasError())
	{
		std::cout << "status=" << parser.getErrorStatus() << std::endl;
	}

	return (0);
}
