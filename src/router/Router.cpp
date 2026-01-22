#include "router/Router.hpp"
#include "utils.hpp"
#include <sstream>
#include <stdlib.h>
#include "router/PathUtils.hpp"

Router::Router(const Config& cfg, const ServerBlock* serverBlock)
	: cfg(cfg), server(serverBlock), rules(NULL) {}

Router::~Router() {}

// /**
//  * @brief Finds the `ServerBlock` that matches the client connection port.
//  * If found, makes a copy of it in `this->server`
//  */
// bool	Router::getServer() {
// 	for (size_t i=0; i < cfg.servers.size(); ++i) {
// 		if (cfg.servers[i].port == this->clientPort) {
// 			this->server = &cfg.servers[i];
// 			return true;
// 		}
// 	}
// 	return false;
// }


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
	// printParentPaths(parentPaths);//		Uncomment to print all parent paths
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

	const LocationBlock* fallback = NULL;
// iterate through parent paths from longest to shortest
	for (DescendingStrSet::iterator it=paths.begin(); it != paths.end(); ++it)
	{
		std::string	longestUri = *it;

		//check if longest URI matches any location block in server
		for (size_t i=0; i < server->locations.size(); ++i) {
			//store index of fallback LocationBlock
			if (server->locations[i].uri == "/")
				fallback = &server->locations[i];
			if (server->locations[i].uri == longestUri) {
				this->rules = &server->locations[i];
				return;
			}
		}
	}
// No matches found, fallback to '/', validate existence of file later
	if (fallback)
		this->rules = fallback;
// No "/" Location Block, build default one from ServerBlock
	else {
		this->defaultLoc = LocationBlock(*server);
		this->rules = &defaultLoc;
	}
}


bool	Router::methodAllowed(const HttpMethod& method) {
	std::string	target;

//	Convert enum to string
	if (method == METHOD_GET)
		target = "GET";
	else if (method == METHOD_DELETE)
		target = "DELETE";
	else if (method == METHOD_POST)
		target = "POST";
	else
		target = "NOT IMPLEMENTED";

	for (size_t i=0; i < rules->methods.size(); ++i) {
		if (target == rules->methods[i])
			return true;
	}
	return false;
}


bool	Router::exceedsMaxSize(const size_t& len) {
	if (len >= rules->clientMaxBodySize)
		return true;
	return false;
}


/**
 * @brief Validates all Routing is correct for a given HTTP request
 * @note checks: Port, URI, Max Size.
 */
RouteResult	Router::routing(const HttpRequest& req) {

// Find correct context for requestURI
	getLocation(req.path);
	if (!rules)// should never happen, defensive coding
		return (RouteResult(500, "Internal Server Error"));

// TODO: a function that matches redirection code with
//       the appropriate redirection message and stores
//       the locationBlock pointer of the request so we
//		 can find rules->redirectTarget when build HttpResponse
//	if (rules->hasRedirect)
//		return buildRedirectRoute(rules->redirectCode);

// Basic Validation Before handling requested method


	if (!methodAllowed(req.method))
		return RouteResult(405, "Method Not Allowed");

	if (exceedsMaxSize(req.contentLength))
		return RouteResult(413, "Payload Too Large");

// Split logic to handle GET, DELETE and POST separately
	if (req.method == METHOD_GET)
		return handleGet(req.path);

	// else if (req.method == METHOD_DELETE)
		//return handleDelete(path);

	// else if (req.method == METHOD_POST)
		//return handlePost(path);

	return RouteResult(501, "Not Implemented");
}



//helper function to read file into string
// static bool readFileToString(const std::string& path, std::string& out)
// {
// 	std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);

// 	if (!ifs.is_open())
// 	{
// 		return false;
// 	}

// 	std::ostringstream oss;
// 	oss << ifs.rdbuf();
// 	out = oss.str();
// 	return true;
// }

HttpResponse Router::buildResponse(const HttpRequest& req) {
	//Kept RouteResult as may need Location pointer for POST so we can provide the path of where the upload occured

	HttpResponse	resp;
	RouteResult		result = routing(req);


	if (result.isSuccess()) {
		std::cout << BOLD_GREEN << result.statusCode << " " << RES << result.errorMsg << std::endl;
		// Build valid response here!
	}
	else {
		std::cout << BOLD_RED << result.statusCode << " "
			<< RES << result.errorMsg << std::endl;
		// Build error response here!
	}


	return resp;
}
