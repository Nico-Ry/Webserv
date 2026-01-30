#include "router/Router.hpp"
#include "utils.hpp"
#include <sstream>
#include <stdlib.h>
#include "router/PathUtils.hpp"
#include "cgi/CgiHandler.hpp"

Router::Router(const Config& cfg, const ServerBlock* serverBlock)
	: cfg(cfg), server(serverBlock), rules(NULL) {}

Router::~Router() {}



/**
 * @brief Generates a set filled with all parent paths in descending order (longest to shortest).
 * @note Such that: `/www/images/data` would produce: `/www/images/data`, `/www/images`, `/www` and `/`
 */
DescendingStrSet	Router::genParentPaths(const std::string& uri)
{
	DescendingStrSet	parentPaths;

// start by inserting root as fallback
	parentPaths.insert("/");

// defensive coding in case empty URI
	if (uri.empty())
		return (parentPaths);

// Read URI char by char inserting everything that's been read
// into buf. Once "/" is encountered insert buf in parentPaths.

	std::string	buf;
	for (size_t i=0; i < uri.length(); ++i)
	{
		char c = uri[i];

		if (c == '/' && buf.length() > 1)// insert if longer than "/"
				parentPaths.insert(buf);
		buf += c;
	}

	parentPaths.insert(uri);//				insert full URI
	// printParentPaths(parentPaths);//		Uncomment to print all parent paths
	return (parentPaths);
}

/**
 * @brief finds the longest matching Location Block for the given URI.
 * @attention Just because no location blocks match the URI does not mean the
 * file/directory doesn't exist. In case no matches are found, we fallback to '/'.
 * Actual validation of existence will be performed later.
 */
void	Router::getLocation(const std::string& uri)
{

	DescendingStrSet	paths = genParentPaths(uri);

	const LocationBlock* fallback = NULL;
// iterate through parent paths from longest to shortest
	for (DescendingStrSet::iterator it=paths.begin(); it != paths.end(); ++it)
	{
		std::string	longestUri = *it;

		//check if longest URI matches any location block in server
		for (size_t i=0; i < server->locations.size(); ++i)
		{
			//store index of fallback LocationBlock
			if (server->locations[i].uri == "/")
				fallback = &server->locations[i];
			if (server->locations[i].uri == longestUri)
			{
				this->rules = &server->locations[i];
				return ;
			}
		}
	}
// No matches found, fallback to '/', validate existence of file later
	if (fallback)
		this->rules = fallback;
// No "/" Location Block, build default one from ServerBlock
	else
	{
		this->defaultLoc = LocationBlock(*server);
		this->rules = &defaultLoc;
	}
}


bool	Router::methodAllowed(const HttpMethod& method)
{
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

	for (size_t i=0; i < rules->methods.size(); ++i)
	{
		if (target == rules->methods[i])
			return (true);
	}
	return (false);
}


bool	Router::exceedsMaxSize(const size_t& len)
{
	if (len >= rules->clientMaxBodySize)
		return (true);
	return (false);
}



// helper function to read file into string
bool	Router::readFileToString(const std::string& path, std::string& responseBody)
{
	// std::ios::in		= flag for open in read mode
	// std::ios::binary	= flag for read raw bytes as they are -> do not auto-modify "\r\n" to "\n"
	std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);


// checks file exists, has read permission and path is valid
	if (!ifs.is_open())
		return (false);

// read and copy over
	std::ostringstream oss;
	oss << ifs.rdbuf();
	responseBody = oss.str();
	return (true);
}



HttpResponse	Router::buildRedirectResponse(const int& code, const std::string& target)
{
	std::string	statusMsg;

	if (code == 301)
		statusMsg = "Moved Permanently";
	else if (code == 302)
		statusMsg = "Found";
	else if (code == 303)
		statusMsg = "See Other";
	else//	if no code matches return internal server error
		return (HttpResponse(500, "Internal Server Error"));

	return (HttpResponse(target, code, statusMsg));
}



/**
 * @brief Validates all Routing is correct for a given HTTP request
 * @note checks: Port, URI, Max Size.
 */
HttpResponse	Router::routing(const HttpRequest& req)
{

// Find correct context for requestURI
	getLocation(req.path);
	if (!rules)// should never happen, defensive coding
		return (HttpResponse(500, "Internal Server Error"));

// ==========================================================================
// CGI HANDLING (checked BEFORE redirects and method validation)
// - CGI scripts use a direct path (./cgi-bin/script.py)
// - CGI bypasses location rules (methods, redirects) to stay independent
// - Only maxBodySize is checked for security
// ==========================================================================
	if (req.path.compare(0, 9, "/cgi-bin/") == 0 && CgiHandler::isCgiScript(req.path))//changed by nico Now /upload/test.py will NOT be treated as CGI, and your upload code will work.
	{
		if (exceedsMaxSize(req.body.size()))
			return (HttpResponse(413, "Payload Too Large"));
		std::string scriptPath = "." + req.path;  // ./cgi-bin/script.py
		std::cout << YELLOW << "[DEBUG - CGI] " << BOLD_BLUE
				  << "Executing CGI: " << scriptPath << RES << std::endl;
		return (CgiHandler::execute(req, scriptPath));
	}

// TODO: a function that matches redirection code with
//       the appropriate redirection message and stores
//       the locationBlock pointer of the request so we
//		 can find rules->redirectTarget when build HttpResponse
	if (rules->hasRedirect)
		return (buildRedirectResponse(rules->redirectCode, rules->redirectTarget));

// Basic Validation Before handling requested method

	if (!methodAllowed(req.method))
		return (HttpResponse(405, "Method Not Allowed"));

	if (exceedsMaxSize(req.body.size()))//changed to bodySize to handle chunked requests
		return (HttpResponse(413, "Payload Too Large"));

// Split logic to handle GET, DELETE and POST separately
	if (req.method == METHOD_GET)
		return (handleGet(req.path));

	else if (req.method == METHOD_DELETE)
		return (handleDelete(req.path));

	else if (req.method == METHOD_POST)
		return (handlePost(req));

	return (HttpResponse(501, "Not Implemented"));
}


HttpResponse Router::buildResponse(const HttpRequest& req)
{
	//Kept HttpResponse as may need Location pointer for POST so we can provide the path of where the upload occured

	HttpResponse		result = routing(req);

	if (result.isSuccess())
	{
	std::cout << BOLD_GREEN << result.statusCode
		<< RES << " " << result.reason << std::endl;
	}
	else
	{
	std::cout << BOLD_RED << result.statusCode
		<< RES << " " << result.reason << std::endl;
	}

	return (result);

	// if (result.isSuccess()) {
	// 	std::cout << BOLD_GREEN << result.statusCode << " " << RES << result.errorMsg << std::endl;
	// 	// Build valid response here!
	// }
	// else {
	// 	std::cout << BOLD_RED << result.statusCode << " "
	// 		<< RES << result.errorMsg << std::endl;
	// 	// Build error response here!
	// }


	// return resp;
}
