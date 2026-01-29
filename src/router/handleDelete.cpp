#include "router/Router.hpp"

static std::string	getParentDirectory(const std::string& filePath) {
	size_t lastSlash = filePath.rfind("/");
	if (lastSlash == std::string::npos || lastSlash == 0)
		return "/";
	return (filePath.substr(0, lastSlash));
}


static bool	canTraverseAndWriteDir(const std::string& p) {
	return (access(p.c_str(), W_OK) == 0 && access(p.c_str(), X_OK) == 0);
}


HttpResponse	Router::handleDelete(const std::string& urlPath) {
	std::string resolvedPath = getResolvedPath(urlPath, *rules);
	std::cout << YELLOW << "[DEBUG - DELETE] " << BOLD_BLUE
			  << "ResolvedPath: " << resolvedPath
			  << RES << std::endl;

	if (!exists(resolvedPath))
		return (HttpResponse(404, "Not Found"));

	if (isDir(resolvedPath))
		return (HttpResponse(403, "Forbidden"));


	std::string	parentDir = getParentDirectory(resolvedPath);
	std::cout << YELLOW << "[DEBUG - DELETE] " << GREEN
			  << "File's Parent Directory: " << parentDir
			  << RES << std::endl;

	if (!canTraverseAndWriteDir(parentDir)) {
		std::cout << YELLOW << "[DELETE] " << RES << "Missing W_OK or X_OK" << std::endl;
		return (HttpResponse(403, "Forbidden"));
	}

	if (unlink(resolvedPath.c_str()) != 0)
		return HttpResponse(500, "Internal Server Error");

	return (HttpResponse(204, "No Content"));
}