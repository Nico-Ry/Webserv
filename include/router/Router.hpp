#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "http/Request.hpp"
#include "http/HttpResponse.hpp"
#include "http/RequestParser.hpp"
#include "configParser/Config.hpp"
#include "configParser/LocationBlock.hpp"
#include "configParser/ServerBlock.hpp"
#include "router/PathUtils.hpp"
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
 * @attention Obsolete! use HttpResponse directly
 * @param isRedirect used as flag, as special HTTP Response behaviour
 * applies for redirections.
 * @param statusCode HTTP status code.
 * @param errorMsg HTTP message that matches `statusCode`
 * @param location the location matching the HTTP Request
 * @param resolvedPath
 */
// struct RouteResult {
// 	bool			isRedirect; //used to determine if redirect, as special behaviour applies
// 	int				statusCode;
// 	std::string		errorMsg;
// 	LocationBlock*	location;
// 	std::string		resolvedPath; // filesystem path of file to serve (GET)

// 	RouteResult(int code)
// 		: statusCode(code), errorMsg(""), location(NULL), resolvedPath("") {}

// 	RouteResult(int code, const std::string& msg)
// 		: statusCode(code), errorMsg(msg), location(NULL), resolvedPath("") {}

// 	RouteResult(int code, const std::string& msg, const std::string& path)
// 		: statusCode(code), errorMsg(msg), location(NULL), resolvedPath(path) {}

// 	bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
// };



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
	HttpResponse		buildRedirectResponse(const int& code, const std::string& target);
	HttpResponse		routing(const HttpRequest& req);


//					 	REQUEST VALIDATION

	bool				getServer();
	void				getLocation(const std::string& uri);
	DescendingStrSet	genParentPaths(const std::string& uri);
	bool				methodAllowed(const HttpMethod& method);
	bool				exceedsMaxSize(const size_t& len);

//---------------------------------------------------------------------------//
//---------------------------- METHOD HANDLERS --------------------------------//
//---------------------------------------------------------------------------//

		/*
			Handle HTTP GET request.
			Input is the parsed URL path (req.path), without query string.
		*/
	HttpResponse	handleGet(const std::string& urlPath);
	bool			readFileToString(const std::string& path, std::string& responseBody);
	HttpResponse	getServeFile(const std::string& resolvedPath);
	HttpResponse	getTryIndexFiles(const std::string& resolvedPath,
						const std::vector<std::string>& indexList);
	HttpResponse	getHandleDirectory(const std::string& resolvedPath,const std::string& requestedPath,
						const LocationBlock& rules);



//								DELETE

	HttpResponse	handleDelete(const std::string& urlPath);




//								POST

		// HttpResponse	handlePost(const HttpRequest& req);
};


std::string	getResolvedPath(const std::string& requestURI, const LocationBlock& rules);


// fsDirPath: filesystem directory path (real path on disk)
//            used to read directory entries and file metadata
// urlPath:   requested URL path (used to generate HTML links)
HttpResponse buildAutoIndexResponse( const std::string& fsDirPath, const std::string& urlPath);

#endif
