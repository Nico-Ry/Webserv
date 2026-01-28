#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "ServerBlock.hpp"

typedef std::vector<std::string> StringVec;
class ServerBlock;

/**
 * @brief Class that stores all information stored between the opening
 * and closing braces of the `location` directive in the config file
 * @attention For any member that ServerBlock and LocationBlock have in common,
 * if the request is for a specific URI (Location), then Location always takes
 * precedence over the Server. If the member was not defined in the config for
 * the given Location, then we fallback to the Server and check the Server's info
 * @param uri The Universal Resource Identifier
 * @param methods Allowed HTTP methods for this URI (stored in vector)
 * @param root Where to look in our file system (root + URI = path)
 * @param index default file to feed for GET requests
 * @param autoIndex Enables/disables directory listing if GET requested a directory
 * @param clientMaxBodySize The max size allowed for any client HTTP request
 * @param errorPages map that contains pairs of int (HTTP status code) and a vector
 * of strings (all files to try and return with the given status code)
 * @param hasRedirect Indicates whether a redirecction was defined
 * in the config file or not
 * @param redirectCode The HTTP status Code linked to the redirect as
 * defined in the config file. Set to 0 by default
 * @param redirectTarget File to serve for the given redirectCode
 */
class LocationBlock {
	public:
		LocationBlock();
		LocationBlock(const ServerBlock& s);
		~LocationBlock();

		std::string					uri;
		std::vector<std::string>	methods;

		std::string					root;
		std::vector<std::string>	index;
		bool						autoIndex;
		std::map<int, StringVec>	errorPages;
		size_t						clientMaxBodySize;

		bool						hasRedirect;
		int							redirectCode;
		std::string					redirectTarget;

		std::string					uploadDir;
};

#endif