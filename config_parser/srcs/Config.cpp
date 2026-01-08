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


static void	pushAndFlush(std::vector<Token>& tokens, TokenType t, std::string& buf, size_t line, size_t col) {
	tokens.push_back(Token(t, buf, line, col));
	buf.clear();
}

void	Config::tokeniseConfig(std::vector<Token>& tokens) {
    std::string currentLine;
    size_t lineNum = 0;

    while (std::getline(cfgFile, currentLine)) {
        lineNum++;
		std::string	tokenBuf;
		size_t		tokenStartCol = 0;

        for (size_t col = 0; col < currentLine.length(); ++col) {
            char c = currentLine[col];

            // Check if special character
			if (c == '{' || c == '}' || c == ';') {
				if (!tokenBuf.empty())
					pushAndFlush(tokens, TOKEN_WORD, tokenBuf, lineNum, tokenStartCol);
				TokenType	type = identifyDelimiter(c);
				tokens.push_back(Token(type, std::string(1, c), lineNum, col));
			}

			// Check if whitespace
			else if (std::isspace(c)) {
				if (!tokenBuf.empty())
					pushAndFlush(tokens, TOKEN_WORD, tokenBuf, lineNum, tokenStartCol);
			}

            // it's a regular character add to buffer
			else {
				if (tokenBuf.empty()) // new word set tokenStartCol to current col
					tokenStartCol = col;
				tokenBuf += c;
			}
		}
        // End of line - flush buffer if not empty
		if (!tokenBuf.empty())
			pushAndFlush(tokens, TOKEN_WORD, tokenBuf, lineNum, tokenStartCol);
	}
}


void Config::parseConfigFile() {
	std::vector<Token>	tokens;
	tokeniseConfig(tokens);
	for (size_t i=0; i < tokens.size(); ++i)
		std::cout << "[" << tokens[i].value << "]" << std::setw(25 - tokens[i].value.size()) << "type: " << tokens[i].type << std::endl;
	//parseTokens(); //using this->tokens
}
