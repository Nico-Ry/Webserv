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
HttpResponse	Router::handleGet(const std::string& requestedPath)
{
	std::string resolvedPath = getResolvedPath(requestedPath, *rules);
	std::cout << YELLOW<<"[DEBUG] " << BOLD_BLUE <<"ResolvedPath " << resolvedPath << RES << std::endl;

	// 1) not found
	if (!exists(resolvedPath)){
		std::cout << ORANGE << "DOES NOT EXIST! " << RES << resolvedPath << std::endl;
		return HttpResponse(404, "Not Found");
	}

	// 2) regular file -> serve it
	if (isFile(resolvedPath))
	{
		return HttpResponse(200, "OK");
	}

	// 3) directory -> try index files
	if (isDir(resolvedPath))
	{
		for (size_t i = 0; i < rules->index.size(); ++i)
		{
			std::string candidate = joinPath(resolvedPath, rules->index[i]);

			if (isFile(candidate))
			{
				return HttpResponse(200, "OK");
			}
		}

		// No autoindex yet, so directory without index is forbidden
		return HttpResponse(403, "Forbidden");
	}

	// Unknown file type -> treat as not found
	return HttpResponse(404, "Not Found");
}
