#include "router/PathUtils.hpp"

/*
	Strip the matched location URI prefix from a URL path.

	Example:
		urlPath = "/kapouet/pouic/toto"
		locUri  = "/kapouet"
		result  = "/pouic/toto"

	If the URL exactly matches the location:
		urlPath = "/kapouet"
		result  = "/"

	This function assumes that the location matching phase
	already guaranteed that urlPath starts with locUri.
	The iterator-based approach avoids relying on substring
	length assumptions and makes the prefix stripping explicit.
*/
std::string	stripLocationPrefixIter(const std::string& urlPath,
											const std::string& locUri)
{
	// If location is "/", do not strip anything.
	if (locUri.empty() || locUri == "/")
		return urlPath;

	std::string::const_iterator itPath = urlPath.begin();
	std::string::const_iterator itLoc  = locUri.begin();

	// Advance both iterators while characters match
	while (itPath != urlPath.end()
		&& itLoc  != locUri.end()
		&& *itPath == *itLoc)
	{
		++itPath;
		++itLoc;
	}

	// If we didn't consume all of locUri, locUri is not a true prefix.
	// This should not happen if getLocation() was correct, but keep it safe.
	if (itLoc != locUri.end())
		return urlPath;

	// If URL equals the location exactly, map to "/" inside the root.
	if (itPath == urlPath.end())
		return "/";

	// Return remaining suffix of the URL path
	return std::string(itPath, urlPath.end());
}


/*
	Join two filesystem path components safely.

	This function ensures that:
	- exactly one '/' exists between path segments
	- no accidental '//' or missing '/' occurs

	Examples:
		"/tmp/www" + "/index.html" -> "/tmp/www/index.html"
		"/tmp/www/" + "index.html" -> "/tmp/www/index.html"
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
		urlPath       = "/kapouet/pouic/toto"
		result        = "/tmp/www/pouic/toto"

	This function centralizes URL → filesystem mapping logic
	and is reused by GET, DELETE, and POST handlers.
*/
std::string	mapUrlToFileSystem(const std::string& urlPath, const LocationBlock& rules)
{
	std::string suffix = stripLocationPrefixIter(urlPath, rules.uri);
	return joinPath(rules.root, suffix);
}
