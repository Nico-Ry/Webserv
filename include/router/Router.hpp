#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../configParser/Config.hpp"
#include "../configParser/LocationBlock.hpp"
#include "../configParser/ServerBlock.hpp"
#include <set>
#include <vector>
#include <iostream>
#include <iomanip>
#include "colours.hpp"

class Config;
struct HttpRequest;
struct HttpResponse;


// Set of strings ordered in desccending order (longest to shortest path)
typedef std::set<std::string, std::greater<std::string> > DescendingStrSet;


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

	//				MEMBERS

		const Config&	cfg;//			Config as built by configParser
		const int&		clientPort;//	Port attached to HTTP request
		Context			rules;//		rules for the given server and location

		ServerBlock		server;//		copy of ServerBlock matching HTTP request
		LocationBlock	location;//		copy of LocationBlock matching HTTP request

	//				FUNCTIONS

		HttpResponse		buildResponse(const HttpRequest& req);
		RouteResult			routing(const HttpRequest& req);
		bool				getServer();
		void				getLocation(const std::string& uri);
		DescendingStrSet	genParentPaths(const std::string& uri);
};

#endif