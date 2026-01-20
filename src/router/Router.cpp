#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "http/Response.hpp"
#include "router/Router.hpp"
#include "utils.hpp"
#include <sstream>
#include <stdlib.h>

Router::Router(const Config& cfg, const int& port) : cfg(cfg), clientPort(port) {}

Router::~Router() {}

/**
 * @brief Finds the `ServerBlock` that matches the client connection port.
 * If found, makes a copy of it in `this->server`
 */
bool	Router::getServer() {
	for (size_t i=0; i < cfg.servers.size(); ++i) {
		if (cfg.servers[i].port == this->clientPort) {
			this->server = cfg.servers[i];
			return true;
		}
	}
	return false;
}


/**
 * @brief Genrates a set filled with all parent paths in descending order (longest to shortest).
 * @note Such that: `/www/images/data` would produce: `/www/images/data`, `/www/images`, `/www` and `/`
 */
DescendingStrSet	Router::genParentPaths(const std::string& uri) {
	DescendingStrSet	parentPaths;

// start by inserting root as fallback
	parentPaths.insert("/");

// defensive coding in case empty URI
	if (uri.empty())
		return parentPaths;

// Read URI char by char inserting everything that's been read
// into buf. Once "/" is encountered insert buf in parentPaths.

	std::string	buf;
	for (size_t i=0; i < uri.length(); ++i) {
		char c = uri[i];

		if (c == '/' && buf.length() > 1)// insert if longer than "/"
				parentPaths.insert(buf);
		buf += c;
	}

	parentPaths.insert(uri);//				insert full URI
	printParentPaths(parentPaths);
	return parentPaths;
}

/**
 * @brief finds the longest matching Location Block for the given URI.
 * @attention Just because no location blocks match the URI does not mean the
 * file/directory doesn't exist. In case no matches are found, we fallback to '/'.
 * Actual validation of existence will be performed later.
 */
void	Router::getLocation(const std::string& uri) {

	DescendingStrSet	paths = genParentPaths(uri);

	size_t fallback = 0;
// iterate through parent paths from longest to shortest
	for (DescendingStrSet::iterator it=paths.begin(); it != paths.end(); ++it)
	{
		std::string	longestUri = *it;

		//check if longest URI matches any location block in server
		for (size_t i=0; i < server.locations.size(); ++i) {
			//store index of fallback LocationBlock
			if (server.locations[i].uri == "/")
				fallback = i;
			if (server.locations[i].uri == longestUri) {
				this->location = server.locations[i];
				return;
			}
		}
	}
// No matches found, fallback to '/', validate existence of file later
	if (fallback)
		this->location = server.locations[fallback];
// No "/" Location Block, build default one from ServerBlock
	else
		this->location = LocationBlock(server);
}


/**
 * @brief Validates all Routing is correct for a given HTTP request
 * @note checks: Port, URI, etc.
 */
RouteResult	Router::routing(const HttpRequest& req) {

	std::string	uri = req.rawTarget;

	if (!getServer())
		return RouteResult(500, "No Server configured for this port");
	getLocation(uri);
	


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