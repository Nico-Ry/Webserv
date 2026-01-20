#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../configParser/Config.hpp"
#include "../configParser/LocationBlock.hpp"
#include "../configParser/ServerBlock.hpp"
#include <iostream>
#include <iomanip>
#include "colours.hpp"

class Config;
struct HttpRequest;
struct HttpResponse;

/**
 * @brief Stores resolved context for a given URI.
 * (Location rules take priority over those of Server)
 */
struct Context {
	std::vector<std::string>	methods;
	std::vector<std::string>	index;
	std::string					root;
	std::map<int, StringVec>	errorPages;
	bool						autoIndex;
	size_t						maxSize;
};


/**
 * @brief Keeps track of status code and potential error message
 * resulting from the routing logic. Has easy to use constructors too.
 */
struct RouteResult {
    int statusCode;
    std::string errorMsg;
    LocationBlock* location;

    RouteResult(int code) : statusCode(code), errorMsg(""), location(NULL) {}
    RouteResult(int code, const std::string& msg) : statusCode(code), errorMsg(msg), location(NULL) {}

    bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
};


class Router {

	public:
		Router(const Config& cfg, const int& port);
		~Router();

		const Config&	cfg;
		const int&		clientPort;//	Port attached to HTTP request
		Context			rules;//		rules for the given server and location

		ServerBlock		server;//		copy of ServerBlock matching HTTP request
		// LocationBlock	location;//		copy of LocationBlock matching HTTP request

		HttpResponse	buildResponse(const HttpRequest& req);

		RouteResult		routing(const HttpRequest& req);
		bool			getServer();
		bool			getLocation(const std::string& uri);
};

#endif