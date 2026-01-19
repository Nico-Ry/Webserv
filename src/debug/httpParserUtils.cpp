#include "utils.hpp"
#include "colours.hpp"
#include "http/Request.hpp"


void	printHttpMethod(const HttpMethod& method) {
	if (method == METHOD_GET)
		std::cout << CYAN << "GET ";
	else if (method == METHOD_DELETE)
		std::cout << CYAN << "DELETE ";
	else if (method == METHOD_POST)
		std::cout << CYAN << "POST ";
	else
		std::cout << RED << "UNKOWN OR UNDEFINED METHOD " << RES;
}

void	printHttpRequest(const HttpRequest& req) {
	std::cout << BOLD_YELLOW << "[DEBUG] http Request Parser:" << RES << std::endl;

// print First line of request
	printHttpMethod(req.method);
	std::cout << CYAN << req.rawTarget << " " << req.httpVersion << std::endl;

// print Headers
	std::cout << RES << "-----------------------------------------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
		std::cout << MAGENTA << it->first << RES << ":" << GREEN << it->second << std::endl;
	std::cout << RES << "-----------------------------------------" << std::endl;

// print request body
	std::cout << PURPLE << req.body << RES << std::endl;
}