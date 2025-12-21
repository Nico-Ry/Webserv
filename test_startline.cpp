#include <iostream>
#include "http/RequestParser.hpp"

/*
	Test program for Phase 3.2 (start line + headers)

	A complete HTTP request must end headers with an empty line:
	"\r\n\r\n"

	So we feed:
	- start line
	- at least 1 header
	- blank line
*/

int	main()
{
	HttpRequestParser	parser;

	std::string	part1 = "GET /index.html HTTP/1.1\r\nHo";
	std::string	part2 = "st: localhost\r\nUser-Agent: curl/8.0\r\n\r\n";

	parser.feed(part1);

	std::cout << "After part1:" << std::endl;
	std::cout << "	isDone = " << parser.isDone() << std::endl;
	std::cout << "	hasError = " << parser.hasError() << std::endl;

	parser.feed(part2);

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

		/*
			Headers are stored lowercased in the parser:
			"Host" becomes "host"
		*/
		std::cout << "	host = " << req.headers.find("host")->second << std::endl;
		std::cout << "	user-agent = " << req.headers.find("user-agent")->second << std::endl;
	}
	else if (parser.hasError())
	{
		std::cout << "Parser error, HTTP status = "
				  << parser.getErrorStatus() << std::endl;
	}

	return (0);
}
