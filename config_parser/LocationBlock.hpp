#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>
#include <map>

class LocationBlock {
	private:
		bool						autoIndex;
		long						clientMaxBodySize;
		std::string					uri;
		std::string					root;
		std::vector<std::string>	index;
		std::vector<std::string>	methods;
		std::map<int, std::string>	errorPages;
		//std:::string				redirection;
	public:
		LocationBlock();
		~LocationBlock();
};

#endif