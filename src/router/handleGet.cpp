#include "router/Router.hpp"
#include "router/PathUtils.hpp"

// RouteResult	Router::handleGet(const std::string& uri) {
// 	// GET LOGIC GOES HERE


// 	(void)uri;
// 	RouteResult	success(200, "OK");
// 	return success;
// }

#include <sys/stat.h>

static bool exists(const std::string& p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		return (false);
	return (true);
}

static bool isDir(const std::string& p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

static bool isFile(const std::string& p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		return (false);
	return (S_ISREG(st.st_mode));
}

RouteResult Router::handleGet(const std::string& urlPath)
{
	// rules must already be set by routing() via getLocation()
	if (!rules)
		return (RouteResult(500, "Internal Server Error"));

	// Redirect handling (if you want GET to respect redirects)
	if (rules->hasRedirect)
		return (RouteResult(rules->redirectCode, "Redirect"));

	std::string fsPath = mapUrlToFs(urlPath, *rules);
	std::cout << YELLOW<<"[DEBUG] "<<BOLD_BLUE<<"Mapped URL path '" << urlPath << "' to filesystem path '" << fsPath << "'\n";

	if (!exists(fsPath))
		return (RouteResult(404, "Not Found"));

	if (isFile(fsPath))
		return (RouteResult(200, "OK"));

	if (isDir(fsPath))
	{
		// For now, just say "directory exists".
		// Next: try index files, then autoindex.
		if (rules->autoIndex)
			return (RouteResult(200, "OK"));
		return (RouteResult(403, "Forbidden"));
	}

	return (RouteResult(404, "Not Found"));
}
