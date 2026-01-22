#ifndef PATHUTILS_HPP
#define PATHUTILS_HPP

#include <string>
#include "../configParser/LocationBlock.hpp"

/*
	Strip the matched location URI prefix from a URL path using iterators.

	Example:
		urlPath="/kapouet/pouic/toto", locUri="/kapouet" -> "/pouic/toto"
	If urlPath equals locUri exactly -> "/"
*/
std::string	stripLocationPrefixIter(const std::string& urlPath,
									const std::string& locUri);

/*
	Join 2 filesystem path components safely so there is exactly 1 '/' between.
*/
std::string	joinPath(const std::string& a, const std::string& b);

/*
	Map a URL path to a filesystem path using location rules:
		fs = rules.root + (urlPath without rules.uri prefix)
*/
std::string	mapUrlToFs(const std::string& urlPath, const LocationBlock& rules);

#endif
