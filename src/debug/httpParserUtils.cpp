#include "utils.hpp"
#include "colours.hpp"
#include "http/Request.hpp"


void	printHttpMethod(const HttpMethod& method)
{
	std::cout << CYAN << "[METHOD]" << RES << std::endl;
	if (method == METHOD_GET)
		std::cout << std::setw(8) << "GET";
	else if (method == METHOD_DELETE)
		std::cout << std::setw(8) << "DELETE";
	else if (method == METHOD_POST)
		std::cout << std::setw(8) << "POST";
	else {
		std::cout << std::setw(8) << ORANGE
			<< "UNKOWN OR UNDEFINED METHOD" << RES;
	}
	std::cout << std::endl;
}

void	printHttpTarget(const HttpRequest& req)
{
	std::cout << CYAN << "[TARGET]" << std::endl
		<< std::setw(8) << RES << req.rawTarget << std::endl;
}

void	printHttpVersion(const HttpRequest& req)
{
	std::cout << CYAN << "[HTTP VERSION]" << std::endl
		<< std::setw(8) << RES << req.httpVersion << std::endl;
}

void	printHttpBody(const HttpRequest& req)
{
	std::cout << CYAN << "[HTTP BODY]" << std::endl
		<< std::setw(8) << RES << req.body << std::endl;
}

void	printHttpHeaders(const HttpRequest& req)
{
	std::cout << CYAN << "[HEADERS]" << std::endl;

	std::map<std::string, std::string>::const_iterator it = req.headers.begin();
	for (; it != req.headers.end(); ++it)
		std::cout << std::setw(8)
			<< MAGENTA << it->first
			<< RES << ":"
			<< GREEN << it->second
			<< RES << std::endl;
}

void	printHttpRequest(const HttpRequest& req)
{
	std::cout << BOLD_YELLOW << "\n~ Parsed HTTP Request ~" << RES << std::endl;

// print first line
	printHttpMethod(req.method);
	printHttpTarget(req);
	printHttpVersion(req);

// print Headers
	printHttpHeaders(req);

// print request body
	printHttpBody(req);
	std::cout << std::endl;
}
