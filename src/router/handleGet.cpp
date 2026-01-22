#include "router/Router.hpp"
#include "router/PathUtils.hpp"
#include <sys/stat.h>

// ---------- helpers ----------
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

// ---------- GET ----------
RouteResult Router::handleGet(const std::string& urlPath)
{
	// rules must already be set by routing() via getLocation()
	if (!rules)
		return (RouteResult(500, "Internal Server Error"));

	// Redirect handling (if you want GET to respect redirects)
	if (rules->hasRedirect)
		return (RouteResult(rules->redirectCode, "Redirect"));

	std::string resolvedPath = mapUrlToFileSystem(urlPath, *rules);
	std::cout << YELLOW<<"[DEBUG] "<<BOLD_BLUE<<"Mapped URL path '" << urlPath << "' to filesystem path '" << resolvedPath << "'\n";

	// 1) not found
	if (!exists(resolvedPath))
	{
		return RouteResult(404, "Not Found");
	}

	// 2) regular file -> serve it
	if (isFile(resolvedPath))
	{
		return RouteResult(200, "OK", resolvedPath);
	}

	// 3) directory -> try index files
	if (isDir(resolvedPath))
	{
		for (size_t i = 0; i < rules->index.size(); ++i)
		{
			std::string candidate = joinPath(resolvedPath, rules->index[i]);

			if (isFile(candidate))
			{
				return RouteResult(200, "OK", candidate);
			}
		}

		// No autoindex yet, so directory without index is forbidden
		return RouteResult(403, "Forbidden");
	}

	// Unknown file type -> treat as not found
	return RouteResult(404, "Not Found");
}
