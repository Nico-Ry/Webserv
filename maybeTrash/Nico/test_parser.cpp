#include <iostream>
#include "http/RequestParser.hpp"

int main() {
	HttpRequestParser parser;

	std::string part1 = "GET /index.html HTTP/1.1\r\nHo";
	std::string part2 = "st: localhost\r\nUser-Agent: curl/8.0\r\n\r\n";

	parser.feed(part1);
	std::cout << "After part1: done=" << parser.isDone()
			  << " error=" << parser.hasError() << "\n";

	parser.feed(part2);
	std::cout << "After part2: done=" << parser.isDone()
			  << " error=" << parser.hasError() << "\n";

	if (parser.isDone() && !parser.hasError()) {
		const HttpRequest &req = parser.getRequest();
		std::cout << "Method: " << req.method << "\n";
		std::cout << "Target: " << req.rawTarget << "\n";
		std::cout << "Version: " << req.httpVersion << "\n";
		std::cout << "Host header: " << req.headers.find("host")->second << "\n";
	}
}
