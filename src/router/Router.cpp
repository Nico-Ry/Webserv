#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "http/Response.hpp"
#include "router/Router.hpp"
#include <sstream>
#include <stdlib.h>

Router::Router(const Config& cfg) : cfg(cfg) {}

Router::~Router() {}



bool	Router::isValidRequest(const HttpRequest& req, const int& clientPort) {

	std::string	uri = req.rawTarget;

	// if (!validPort(clientPort))
	// 	return false;


	std::cout << BOLD_YELLOW << "~ isValidRequest ~" << RES << std::endl;
	std::cout << CYAN << "[PORT]\n" << std::setw(8) << RES << clientPort << std::endl;
	std::cout << CYAN << "[URI]\n" << std::setw(8) << RES << uri << std::endl;

	return true;
}

HttpResponse Router::buildResponse(const HttpRequest& req, const int& clientPort) {
	HttpResponse	resp;


	if (isValidRequest(req, clientPort)) {
		std::cout << BOLD_GREEN << "VALID HTTP REQUEST!" << RES << std::endl;
		// Build valid response here!
	}
	else {
		std::cout << BOLD_RED << "REQUEST NOT VALID, PLEASE BUILD ERROR RESPONSE!" << RES << std::endl;
		// Build error response here!
	}

	resp.body = "caca";

	return resp;
}