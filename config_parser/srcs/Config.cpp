/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 10:40:54 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 10:44:41 by ameechan         ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

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
	printTokens(tokens);

// Parse
	// ConfigParser	parser(tokens);
	// parser.parse(result);
	// validate(result);
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