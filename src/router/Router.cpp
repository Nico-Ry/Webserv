#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "http/Response.hpp"
#include "router/Router.hpp"
#include <sstream>
#include <stdlib.h>

Router::Router(const Config& cfg, const int& port) : cfg(cfg), clientPort(port) {}

Router::~Router() {}


bool	Router::getServer() {
	for (size_t i=0; i < cfg.servers.size(); ++i) {
		if (cfg.servers[i].port == this->clientPort) {
			this->server = cfg.servers[i];
			return true;
		}
	}
	return false;
}


RouteResult	Router::routing(const HttpRequest& req) {

	std::string	uri = req.rawTarget;

	if (!getServer())
		return RouteResult(500, "No Server configured for this port");


	std::cout << BOLD_YELLOW << "~ routing ~" << RES << std::endl;
	std::cout << CYAN << "[PORT]\n" << std::setw(8) << RES << clientPort << std::endl;
	std::cout << CYAN << "[URI]\n" << std::setw(8) << RES << uri << std::endl;

	RouteResult	success(200);
	return success;
}

HttpResponse Router::buildResponse(const HttpRequest& req) {
	HttpResponse	resp;
	RouteResult		result = routing(req);


	if (result.isSuccess()) {
		std::cout << BOLD_GREEN << "VALID HTTP REQUEST!" << RES << std::endl;
		// Build valid response here!
	}
	else {
		std::cout << BOLD_RED << "REQUEST NOT VALID, PLEASE BUILD ERROR RESPONSE!" << RES << std::endl;
		// Build error response here!
	}

	// resp.body = "caca";

	return resp;
}