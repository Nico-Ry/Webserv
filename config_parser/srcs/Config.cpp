/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:19 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/07 18:39:10 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {}

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


bool	Config::validInputFile(const std::string& fileName) {
	cfgFile.open(fileName.c_str(), std::ifstream::in);
	if (!cfgFile.is_open()) {
		std::cerr << "Error: " << fileName << ": Failed to open file";
		return false;
	}
	else if (cfgFile.fail()) {
		std::cerr << "Error: " << fileName << ": Could not read from input file";
		return false;
	}
	return true;
}


TokenType	Config::identifyDelimiter(const char& delimiter) {
	if (delimiter == '{')
		return TOKEN_LBRACE;
	if (delimiter == '}')
		return TOKEN_RBRACE;
	if (delimiter == ';')
		return TOKEN_SEMICOLON;

	//here for safety purposes in case a wrong char is sent
	std::cout << "[DEBUG]: identifyDelimiter() expected delimiter but got: "
		<< delimiter << std::endl;
	return TOKEN_EOF;
}


void	Config::tokeniseConfig(std::vector<Token>& tokens) {
    std::string currentLine;
    int lineNum = 0;

    while (std::getline(cfgFile, currentLine)) {
        lineNum++;
		std::string	tokenBuf;
		size_t		tokenStartCol = 0;

        for (size_t col = 0; col < currentLine.length(); ++col) {
            char c = currentLine[col];

            // TODO: Check if special character
			if (c == '{' || c == '}' || c == ';') {
				if (!tokenBuf.empty()) {
					tokens.push_back(Token(TOKEN_WORD, tokenBuf, lineNum, tokenStartCol));
					tokenBuf.clear();
				}
				TokenType	type = identifyDelimiter(c);
				tokens.push_back(Token(type, std::string(1, c), lineNum, col));
			}
            // TODO: Check if whitespace
			else if (std::isspace(c)) {
				if (!tokenBuf.empty()) {
					tokens.push_back(Token(TOKEN_WORD, tokenBuf, lineNum, tokenStartCol));
					tokenBuf.clear();
				}
			}
            // TODO: Otherwise, it's a regular character
			else {
				if (tokenBuf.empty())
					tokenStartCol = col;
				tokenBuf += c;
			}
		}
        // TODO: End of line - flush buffer if needed
		if (!tokenBuf.empty()) {
			tokens.push_back(Token(TOKEN_WORD, tokenBuf, lineNum, tokenStartCol));
			tokenBuf.clear();
		}
	}
	cfgFile.close();
}


void Config::parseConfigFile() {
	std::vector<Token>	tokens;
	tokeniseConfig(tokens);
	for (size_t i=0; i < tokens.size(); ++i)
		std::cout << "[" << tokens[i].value << "] ";
	//parseTokens(); //using this->tokens
}
