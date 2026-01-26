#include "router/PathUtils.hpp"
#include <sys/stat.h>

// ---------- helpers ----------
bool exists(const std::string& p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		return (false);
	return (true);
}

bool isDir(const std::string& p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

bool isFile(const std::string& p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		return (false);
	return (S_ISREG(st.st_mode));
}


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
