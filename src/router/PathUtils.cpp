#include "router/PathUtils.hpp"

/*
	Strip the matched location URI prefix from a URL path.

	Example:
		requestURI = "/kapouet/pouic/toto"
		locUri  = "/kapouet"
		result  = "/pouic/toto"

	If the URL exactly matches the location:
		requestURI = "/kapouet"
		result  = "/"

	This function assumes that the location matching phase
	already guaranteed that requestURI starts with locUri.
	The iterator-based approach avoids relying on substring
	length assumptions and makes the prefix stripping explicit.
*/
// std::string	stripLocationPrefixIter(const std::string& requestURI,
// 											const std::string& locUri)
// {
// 	// If location is "/", do not strip anything.
// 	if (locUri.empty() || locUri == "/")
// 		return requestURI;

// 	std::string::const_iterator itPath = requestURI.begin();
// 	std::string::const_iterator itLoc  = locUri.begin();

// 	// Advance both iterators while characters match
// 	while (itPath != requestURI.end()
// 		&& itLoc  != locUri.end()
// 		&& *itPath == *itLoc)
// 	{
// 		++itPath;
// 		++itLoc;
// 	}

// 	// If we didn't consume all of locUri, locUri is not a true prefix.
// 	// This should not happen if getLocation() was correct, but keep it safe.
// 	if (itLoc != locUri.end())
// 		return requestURI;

// 	// If URL equals the location exactly, map to "/" inside the root.
// 	if (itPath == requestURI.end())
// 		return "/";

// 	// Return remaining suffix of the URL path
// 	return std::string(itPath, requestURI.end());
// }


/*
	Join two filesystem path components safely.

	This function ensures that:
	- exactly one '/' exists between path segments
	- no accidental '//' or missing '/' occurs

	Examples:
		"/tmp/www"	+ "/index.html" -> "/tmp/www/index.html"
		"/tmp/www/" + "index.html" -> "/tmp/www/index.html"
		"tmp/www/"	+ "/index.html"
*/
std::string	joinPath(const std::string& a, const std::string& b)
{
	if (a.empty()) return b;
	if (b.empty()) return a;

	if (a[a.size() - 1] == '/' && b[0] == '/')
		return a + b.substr(1);
	if (a[a.size() - 1] != '/' && b[0] != '/')
		return a + "/" + b;
	return a + b;
}


/*
	Map a URL path to a filesystem path using the matched location rules.

	Process:
	1) Strip the location URI prefix from the URL path
	2) Append the remaining suffix to the location root

	Example:
		location.uri  = "/kapouet"
		location.root = "/tmp/www"
		requestURI       = "/kapouet/pouic/toto"
		result        = "/tmp/www/pouic/toto"

	This function centralizes URL → filesystem mapping logic
	and is reused by GET, DELETE, and POST handlers.
*/
std::string	getResolvedPath(const std::string& requestURI, const LocationBlock& rules)
{
	std::string	path = requestURI;

	if (rules.uri != "/" && requestURI.find(rules.uri) == 0)
		path = requestURI.substr(rules.uri.length());

	return joinPath(rules.root, path);
}
