#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "http/RequestParser.hpp"
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
 * @brief Keeps track of status code and potential error message
 * resulting from the routing logic. Has easy to use constructors too.
 */
struct RouteResult {
	int				statusCode;
	std::string		errorMsg;
	LocationBlock*	location;

	RouteResult(int code) : statusCode(code), errorMsg(""), location(NULL) {}
	RouteResult(int code, const std::string& msg) : statusCode(code), errorMsg(msg), location(NULL) {}

	bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
};


class Router {

	public:
		Router(const Config& cfg, const ServerBlock* serverBlock);
		~Router();

//---------------------------------------------------------------------------//
//								 MEMBERS
//---------------------------------------------------------------------------//

	const Config&		cfg;//			Config as built by configParser

	const ServerBlock	*server;//		Pointer to ServerBlock matching HTTP request
	const LocationBlock	*rules;//		Pointer to LocationBlock matching HTTP request
	LocationBlock		defaultLoc;//	Only used if "/" is not configured in Config

//---------------------------------------------------------------------------//
//								FUNCTIONS
//---------------------------------------------------------------------------//
//						ROUTING

	HttpResponse		buildResponse(const HttpRequest& req);
	RouteResult			routing(const HttpRequest& req);


//					 	REQUEST VALIDATION

	bool				getServer();
	void				getLocation(const std::string& uri);
	DescendingStrSet	genParentPaths(const std::string& uri);
	bool				methodAllowed(const HttpMethod& method);
	bool				exceedsMaxSize(const size_t& len);

//---------------------------- METHOD HANDLERS --------------------------------//

		/*
			Handle HTTP GET request.
			Input is the parsed URL path (req.path), without query string.
		*/
		RouteResult		handleGet(const std::string& urlPath);

		// Future:
		// RouteResult	handleDelete(const std::string& urlPath);
		// RouteResult	handlePost(const HttpRequest& req);
};

#endif
