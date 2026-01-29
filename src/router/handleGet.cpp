
#include "router/PathUtils.hpp"
#include <sys/stat.h>
#include "router/Router.hpp"
// ------------------------------------------------------------
// GET helpers (file-local)
// ------------------------------------------------------------

/*
 * NOTE:
 * handleGet() was refactored into small, file-local helper functions to
 * improve readability and maintainability.
 *
 * Some helpers take a `Router& self` parameter because they need to call
 * Router member functions (e.g. readFileToString), while remaining non-member
 * helpers local to this .cpp file.
 *
 * This preserves Router ownership of core logic while keeping helpers
 * isolated and reusable.
 */


static HttpResponse	getNotFound(const std::string& resolvedPath)
{
	std::cout << YELLOW << "[DEBUG - GET] " << ORANGE
			  << "DOES NOT EXIST! " << RES << resolvedPath
			  << std::endl;
	return (HttpResponse(404, "Not Found"));
}

HttpResponse Router::getServeFile( const std::string& resolvedPath)
{
	std::string body;

	std::cout << YELLOW << "[DEBUG - GET] " << GREEN
			  << "Path links to file" << RES << std::endl;

	// Permission check (chmod impacts this)
	// - chmod 666 -> readable -> OK
	// - chmod 200/000 -> not readable -> 403
	if (!canReadFile(resolvedPath))
	{
		debugAccessError("READ", resolvedPath);
		return (HttpResponse(403, "Forbidden"));
	}

	// Actual read (even if access() said OK, open() can still fail)
	if (!readFileToString(resolvedPath, body))
	{
		std::cout << YELLOW << "[DEBUG - GET] " << ORANGE
				  << "Failed reading file despite R_OK: " << RES
				  << resolvedPath << std::endl;
		return (HttpResponse(500, "Internal Server Error"));
	}

	return (HttpResponse(200, "OK", body));
}


static bool	needsDirRedirect(const std::string& requestedPath)
{
	return (!requestedPath.empty() && requestedPath[requestedPath.size() - 1] != '/');
}

// static HttpResponse	getDirRedirect(const std::string& requestedPath)
// {
// 	// Redirect /dir -> /dir/ so relative links work correctly.
// 	// Your constructor: HttpResponse(location, code, reason)
// 	return (HttpResponse(requestedPath + "/", 301, "Moved Permanently"));
// }

HttpResponse Router::getTryIndexFiles(const std::string& resolvedPath,
	const std::vector<std::string>& indexList)
{
	std::string body;

	// Try index files (index lookup requires traversable dir; file itself must be readable)
	for (size_t i = 0; i < indexList.size(); ++i)
	{
		std::string candidate = joinPath(resolvedPath, indexList[i]);

		std::cout << YELLOW << "[DEBUG - GET] " << RES
				  << "Trying: " << PURPLE << candidate << RES << std::endl;

		if (isFile(candidate))
		{
			if (!canReadFile(candidate))
			{
				debugAccessError("READ index file", candidate);
				return (HttpResponse(403, "Forbidden"));
			}
			if (!readFileToString(candidate, body))
				return (HttpResponse(403, "Forbidden"));
			return (HttpResponse(200, "OK", body));
		}
	}

	// Signal "no index file found" with a special response.
	// We keep this simple: return 0 status means "continue".
	return (HttpResponse(0, ""));
}

/*
 * Sentinel check used internally by GET directory handling.
 *
 * A status code of 0 is not a valid HTTP response and is used here to signal
 * "no index file found, continue directory logic" rather than a real response
 * to return to the client.
 */
static bool	isNoIndexSentinel(const HttpResponse& resp)
{
	return (resp.statusCode == 0);
}

HttpResponse	Router::getHandleDirectory(const std::string& resolvedPath,
	const std::string& requestedPath,
	const LocationBlock& rules)
{
	std::cout << YELLOW << "[DEBUG - GET] " << CYAN
			  << "Path links to directory" << RES << std::endl;

	// Need X permission to traverse. If missing -> 403.
	// Example: chmod 666 on a directory => no X => forbidden.
	if (!canTraverseDir(resolvedPath))
	{
		debugAccessError("TRAVERSE (X)", resolvedPath);
		return (HttpResponse(403, "Forbidden"));
	}

	// Normalize directory URL for correct relative-link behavior.
	if (needsDirRedirect(requestedPath))
		return (HttpResponse(requestedPath + "/", 301, "Moved Permanently"));

	// Try index files first.
	HttpResponse indexResp = getTryIndexFiles(resolvedPath, rules.index);
	if (!isNoIndexSentinel(indexResp))
		return (indexResp);

	// No index file found -> autoindex or forbidden
	if (rules.autoIndex == true)
	{
		std::cout << YELLOW << "[DEBUG - GET] " << RES
				  << "canListDir(" << resolvedPath << ") = "
				  << (canListDir(resolvedPath) ? "true" : "false")
				  << std::endl;

		// Autoindex needs list permissions (R + X) on the directory
		if (!canListDir(resolvedPath))
		{
			debugAccessError("LIST (R+X) directory", resolvedPath);
			return (HttpResponse(403, "Forbidden"));
		}

		return (buildAutoIndexResponse(resolvedPath, requestedPath));
	}

	// Directory exists, no index, autoindex off
	return (HttpResponse(403, "Forbidden"));
}

HttpResponse Router::handleGet(const std::string& requestedPath)
{
	std::string resolvedPath = getResolvedPath(requestedPath, *rules);

	std::cout << YELLOW << "[DEBUG - GET] " << BOLD_BLUE
			  << "ResolvedPath: " << resolvedPath
			  << RES << std::endl;

	// 1) Not found
	if (!exists(resolvedPath))
		return (getNotFound(resolvedPath));

	// 2) Regular file -> serve it
	if (isFile(resolvedPath))
		return (getServeFile( resolvedPath));

	// 3) Directory -> normalize URL, try index files, else autoindex/403
	if (isDir(resolvedPath))
		return (getHandleDirectory(resolvedPath, requestedPath, *rules));

	// Unknown file type (fifo, socket, device, etc.)
	std::cout << YELLOW << "[DEBUG - GET] " << ORANGE
			  << "Unknown file type: " << RES << resolvedPath << std::endl;
	return (HttpResponse(404, "Not Found"));
}


// HttpResponse Router::handleGet(const std::string& requestedPath)
// {
// 	std::string resolvedPath = getResolvedPath(requestedPath, *rules);
// 	std::string body;

// 	std::cout << YELLOW << "[DEBUG] " << BOLD_BLUE
// 			  << "ResolvedPath: " << resolvedPath
// 			  << RES << std::endl;

// 	// 1) Not found
// 	if (!exists(resolvedPath))
// 	{
// 		std::cout << YELLOW << "[DEBUG] " << ORANGE
// 				  << "DOES NOT EXIST! " << RES << resolvedPath
// 				  << std::endl;
// 		return (HttpResponse(404, "Not Found"));
// 	}

// 	// 2) Regular file -> serve it
// 	if (isFile(resolvedPath))
// 	{
// 		std::cout << YELLOW << "[DEBUG] " << GREEN
// 				  << "Path links to file" << RES << std::endl;

// 		// Permission check (chmod impacts this)
// 		// - chmod 666 -> readable -> OK
// 		// - chmod 200/000 -> not readable -> 403
// 		if (!canReadFile(resolvedPath))
// 		{
// 			debugAccessError("READ", resolvedPath);
// 			return (HttpResponse(403, "Forbidden"));
// 		}

// 		// Actual read (even if access() said OK, open() can still fail)
// 		if (!readFileToString(resolvedPath, body))
// 		{
// 			std::cout << YELLOW << "[DEBUG] " << ORANGE
// 					  << "Failed reading file despite R_OK: " << RES
// 					  << resolvedPath << std::endl;
// 			return (HttpResponse(403, "Forbidden"));
// 		}

// 		return (HttpResponse(200, "OK", body));
// 	}

// 	// 3) Directory -> normalize URL, try index files, else autoindex/403
// 	if (isDir(resolvedPath))
// 	{
// 		std::cout << YELLOW << "[DEBUG] " << CYAN
// 				  << "Path links to directory" << RES << std::endl;

// 		// Need X permission to traverse. If missing -> 403.
// 		// Example: chmod 666 on a directory => no X => forbidden.
// 		if (!canTraverseDir(resolvedPath))
// 		{
// 			debugAccessError("TRAVERSE (X)", resolvedPath);
// 			return (HttpResponse(403, "Forbidden"));
// 		}

// 		// Redirect /dir -> /dir/ so relative links work correctly.
// 		if (!requestedPath.empty() && requestedPath[requestedPath.size() - 1] != '/')
// 		{
// 			// Your constructor: HttpResponse(location, code, reason)
// 			return (HttpResponse(requestedPath + "/", 301, "Moved Permanently"));
// 		}

// 		// Try index files (index lookup requires traversable dir; file itself must be readable)
// 		for (size_t i = 0; i < rules->index.size(); ++i)
// 		{
// 			std::string candidate = joinPath(resolvedPath, rules->index[i]);

// 			std::cout << YELLOW << "[DEBUG] " << RES
// 					  << "Trying: " << PURPLE << candidate << RES << std::endl;

// 			if (isFile(candidate))
// 			{
// 				if (!canReadFile(candidate))
// 				{
// 					debugAccessError("READ index file", candidate);
// 					return (HttpResponse(403, "Forbidden"));
// 				}
// 				if (!readFileToString(candidate, body))
// 					return (HttpResponse(403, "Forbidden"));
// 				return (HttpResponse(200, "OK", body));
// 			}
// 		}

// 		// No index file found -> autoindex or forbidden
// 		if (rules->autoIndex == true)
// 		{
// 			std::cout << YELLOW << "[DEBUG] " << RES
// 			<< "canListDir(" << resolvedPath << ") = "
// 			<< (canListDir(resolvedPath) ? "true" : "false")
// 			<< std::endl;
// 			// Autoindex needs list permissions (R + X) on the directory
// 			if (!canListDir(resolvedPath))
// 			{
// 				debugAccessError("LIST (R+X) directory", resolvedPath);
// 				return (HttpResponse(403, "Forbidden"));
// 			}

// 			return (buildAutoIndexResponse(resolvedPath, requestedPath));
// 		}

// 		// Directory exists, no index, autoindex off
// 		return (HttpResponse(403, "Forbidden"));
// 	}

// 	// Unknown file type (fifo, socket, device, etc.) -> treat as not found or forbidden
// 	std::cout << YELLOW << "[DEBUG] " << ORANGE
// 			  << "Unknown file type: " << RES << resolvedPath << std::endl;
// 	return (HttpResponse(404, "Not Found"));
// }

