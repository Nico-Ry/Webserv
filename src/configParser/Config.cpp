#include "configParser/Config.hpp"

Config::Config(const std::string& configFile) {
	std::ifstream file(configFile.c_str());
	if (!file.is_open())
		throw std::runtime_error("Cannot open config file: " + configFile);
	else if (file.fail())
		throw std::runtime_error("Cannot read from config file: " + configFile);

// Tokenize
    std::vector<Token> tokens;
    Tokeniser tokeniser(file);
    tokeniser.tokenise(tokens);
	// printTokens(tokens);

// Parse
	ConfigParser	parser(tokens);
	parser.parse(*this);

	detectDuplicatePorts(this->servers);
}


// Copy Assignment Constructor
Config::Config(const Config& other) {
	this->servers = other.servers;
}

// Assignment Constructor
Config&	Config::operator=(const Config& other) {
if (this != &other) {
	this->servers = other.servers;
}
return *this;
}


void	Config::detectDuplicatePorts(const std::vector<ServerBlock>& servs) {
	std::set<int>	usedPorts;

	for (size_t i=0; i < servs.size(); ++i) {
		int port = servs[i].port;

		if (!usedPorts.insert(port).second) {
			std::stringstream	ss;
			ss << "Duplicate port: " << RED << port
				<< RES << " found in multiple servers";
			throw std::runtime_error(ss.str());
		}
	}
}


Config::~Config() {}


