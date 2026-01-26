#include "router/Router.hpp"
#include "router/PathUtils.hpp"
#include <sys/stat.h>

// ---------- GET ----------
HttpResponse Router::handleGet(const std::string& requestedPath)
{
	std::string resolvedPath = getResolvedPath(requestedPath, *rules);
	std::string body;

	std::cout << YELLOW << "[DEBUG] " << BOLD_BLUE
			  << "ResolvedPath: " << resolvedPath
			  << RES << std::endl;

	// 1) Not found
	if (!exists(resolvedPath))
	{
		std::cout << YELLOW << "[DEBUG] " << ORANGE
				  << "DOES NOT EXIST! " << RES << resolvedPath
				  << std::endl;
		return HttpResponse(404, "Not Found");
	}

	// 2) Regular file -> serve it
	if (isFile(resolvedPath))
	{
		std::cout << YELLOW << "[DEBUG] " << GREEN
				  << "Path links to file" << RES << std::endl;

		// Permission check (chmod impacts this)
		// - chmod 666 -> readable -> OK
		// - chmod 200/000 -> not readable -> 403
		if (!canReadFile(resolvedPath))
		{
			debugAccessError("READ", resolvedPath);
			return HttpResponse(403, "Forbidden");
		}

		// Actual read (even if access() said OK, open() can still fail)
		if (!readFileToString(resolvedPath, body))
		{
			std::cout << YELLOW << "[DEBUG] " << ORANGE
					  << "Failed reading file despite R_OK: " << RES
					  << resolvedPath << std::endl;
			return HttpResponse(403, "Forbidden");
		}

		return HttpResponse(200, "OK", body);
	}

	// 3) Directory -> normalize URL, try index files, else autoindex/403
	if (isDir(resolvedPath))
	{
		std::cout << YELLOW << "[DEBUG] " << CYAN
				  << "Path links to directory" << RES << std::endl;

		// Need X permission to traverse. If missing -> 403.
		// Example: chmod 666 on a directory => no X => forbidden.
		if (!canTraverseDir(resolvedPath))
		{
			debugAccessError("TRAVERSE (X)", resolvedPath);
			return HttpResponse(403, "Forbidden");
		}

		// Redirect /dir -> /dir/ so relative links work correctly.
		if (!requestedPath.empty() && requestedPath[requestedPath.size() - 1] != '/')
		{
			// Your constructor: HttpResponse(location, code, reason)
			return HttpResponse(requestedPath + "/", 301, "Moved Permanently");
		}

		// Try index files (index lookup requires traversable dir; file itself must be readable)
		for (size_t i = 0; i < rules->index.size(); ++i)
		{
			std::string candidate = joinPath(resolvedPath, rules->index[i]);

			std::cout << YELLOW << "[DEBUG] " << RES
					  << "Trying: " << PURPLE << candidate << RES << std::endl;

			if (isFile(candidate))
			{
				if (!canReadFile(candidate))
				{
					debugAccessError("READ index file", candidate);
					return HttpResponse(403, "Forbidden");
				}
				if (!readFileToString(candidate, body))
					return HttpResponse(403, "Forbidden");
				return HttpResponse(200, "OK", body);
			}
		}

		// No index file found -> autoindex or forbidden
		if (rules->autoIndex == true)
		{
			// Autoindex needs list permissions (R + X) on the directory
			if (!canListDir(resolvedPath))
			{
				debugAccessError("LIST (R+X) directory", resolvedPath);
				return HttpResponse(403, "Forbidden");
			}

			return buildAutoIndexResponse(resolvedPath, requestedPath);
		}

		// Directory exists, no index, autoindex off
		return HttpResponse(403, "Forbidden");
	}

	// Unknown file type (fifo, socket, device, etc.) -> treat as not found or forbidden
	std::cout << YELLOW << "[DEBUG] " << ORANGE
			  << "Unknown file type: " << RES << resolvedPath << std::endl;
	return HttpResponse(404, "Not Found");
}

