#include <iostream>
#include "http/RequestParser.hpp"

/*
	Test Phase 3.8: shouldCloseConnection()

	We test 3 cases:

	1) HTTP/1.1 without Connection header => keep (close=false)
	2) HTTP/1.1 with Connection: close    => close=true
	3) HTTP/1.0 with Connection: keep-alive => keep (close=false)
*/

static void	test_case(const std::string &raw, const std::string &name)
{
	HttpRequestParser	parser;

	parser.feed(raw);

	std::cout << name << ":" << std::endl;
	std::cout << "	isDone = " << parser.isDone() << std::endl;
	std::cout << "	hasError = " << parser.hasError() << std::endl;

	if (parser.isDone() && !parser.hasError())
	{
		std::cout << "	shouldCloseConnection = " << parser.shouldCloseConnection() << std::endl;
	}
	else if (parser.hasError())
	{
		std::cout << "	errorStatus = " << parser.getErrorStatus() << std::endl;
	}
}

int	main()
{
	test_case(
		"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n",
		"Case 1 (HTTP/1.1 default)"
	);

	test_case(
		"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n",
		"Case 2 (HTTP/1.1 close)"
	);

	test_case(
		"GET / HTTP/1.0\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n",
		"Case 3 (HTTP/1.0 keep-alive)"
	);

	return (0);
}
