#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include "ConfigParser.hpp"
#include "Tokeniser.hpp"

/**
 * @brief The main object that contains all ServerBlocks, which in turn
 * contain all LocationBlocks within them
 */
class Config {
	public:
//				CONSTRUCTOR & DESTRUCTOR
		Config(const std::string& configFile);
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

// 		vector of all ServerBlocks defined in config file
		std::vector<ServerBlock>	servers;
};

#endif