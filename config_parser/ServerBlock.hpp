#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <string>
#include <vector>
#include <map>

class ServerBlock {
	private:
		bool							autoIndex;
		long							clientMaxBodySize;
		std::string						root;
		std::vector<int>				port;
		std::vector<std::string>		index;
		//std::vector<LocationBlock>	location;
		std::map<int, std::string>		errorPages;
	public:
		ServerBlock();
		~ServerBlock();
		//void	parseConfig(std::fstream& config); // move to bigger base class
};

#endif