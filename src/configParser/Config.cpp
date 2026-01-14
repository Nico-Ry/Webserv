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

Config::~Config() {}


