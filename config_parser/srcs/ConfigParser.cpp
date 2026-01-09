/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:34:06 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 21:36:58 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#pragma region TEMP DEBUG FUNCS

void	printServerPorts(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (data.servers[i].port) {
			std::cout << "[DEBUG] server: " << i
				<< " -> port: " << data.servers[i].port << std::endl;
		}
		else
			std::cout << "[DEBUG] server: " << i << " -> No port!" << std::endl;

	}
}

void	printServerRoot(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (!data.servers[i].root.empty()) {
			std::cout << "[DEBUG] server: " << i
			<< " -> root: " << data.servers[i].root << std::endl;
		}
		else
			std::cout << "[DEBUG] server: " << i << " -> No root!" << std::endl;

	}
}

#pragma endregion TEMP DEBUG FUNCS

ConfigParser::ConfigParser(const std::vector<Token>& toks)
	: tokens(toks), currentIndex(0) {

	//build map for server directives(KEY) to function pointers(VALUE)
		serverDirectives["listen"] = &ConfigParser::parseListen;
		serverDirectives["root"] = &ConfigParser::parseRoot;
	}

ConfigParser::~ConfigParser() {}




ServerBlock	ConfigParser::parseServerBlock() {
	Token		current = expect(TOKEN_WORD, "Expected 'server'");

	if (current.value != "server")
		throw std::runtime_error("Unkown directive: " + current.value);
	current = expect(TOKEN_LBRACE, "Expected '{'");

	ServerBlock	s;
	while (!check(TOKEN_RBRACE)) {
		Token	directive = expect(TOKEN_WORD, "Expected directive");

	// Find pointer to parsing function that matches directive.value
		std::map<std::string, ServerFn>::iterator it	=
			serverDirectives.find(directive.value);

	// If directive not found in map, throw error
		if (it == serverDirectives.end())
			throw std::runtime_error("Unkown directive: " + directive.value);

	// Else call function pointer to parse directive
		(this->*(it->second))(s);
	}
	current = expect(TOKEN_RBRACE, "Expected '}'");
	return s;
}




void	ConfigParser::parse(Config& data) {

	while (!isAtEnd()) {
		data.servers.push_back(parseServerBlock());
	}
	printServerPorts(data);
	std::cout << "-------------------------" << std::endl;
	printServerRoot(data);
}






#pragma region Parse Directives

void	ConfigParser::parseListen(ServerBlock& s) {

	std::stringstream	ss(tokens[currentIndex].value);
	long	port;

	if (!(ss >> port))
		throw std::runtime_error("listen: not a number or out of range: " + peek().value);
	if (!ss.eof())
		throw std::runtime_error("listen: invalid characters: " + peek().value);
	if (port < 1 || port > 65535)
		throw std::runtime_error("listen: port out of range: " + peek().value);

// consume port number
	consume();
	expect(TOKEN_SEMICOLON, "Expected ';'");
	s.port = port;
}

void	ConfigParser::parseRoot(ServerBlock& s) {
	s.root = tokens[currentIndex].value;
	consume();
	expect(TOKEN_SEMICOLON, "Expected ';'");
}

#pragma endregion




#pragma region Helper Funcs

//Look at current Token without consuming it
Token	ConfigParser::peek() const {
	if (currentIndex < tokens.size())
		return tokens[currentIndex];
	return tokens.back();
}

//Look at next Token without consuming it
Token	ConfigParser::peekNext() const {
	if (currentIndex + 1 < tokens.size())
		return tokens[currentIndex + 1];
	return tokens.back();
}

// Consumes and returns current Token
Token	ConfigParser::consume() {
	if (isAtEnd())
		throw(std::runtime_error("Can't consume past end of tokens!"));
	Token	current = peek();
	++currentIndex;
	return current;
}

/**
 * @brief expects a specific token, throws error if not expected type
 * @return if type matches, returns consume()
 */
Token	ConfigParser::expect(TokenType type, const std::string& msg) {
	Token	current = peek();
	if (current.type != type) {
		std::stringstream	ss;
		ss << "Parse Error at line " << current.line
			<< ", column " << current.column
			<< ": " << msg
			<< " (got '" << current.value << "' instead)";
		throw std::runtime_error(ss.str());
	}
	return consume();
}

bool	ConfigParser::isAtEnd() const {
	if (currentIndex >= tokens.size())
		return true;
	return false;
}

// Returns true if current token matches type
bool	ConfigParser::check(TokenType type) const {
	if (peek().type == type)
		return true;
	return false;
}

// If current token matches, consume it and return true
bool	ConfigParser::match(TokenType type) {
	if (check(type)) {
		consume();
		return true;
	}
	return false;
}

// Check if current token is a word with specific value
bool ConfigParser::checkWord(const std::string& value) const {
    Token current = peek();
    return current.type == TOKEN_WORD && current.value == value;
}

// Match and consume if word matches
bool ConfigParser::matchWord(const std::string& value) {
    if (checkWord(value)) {
        consume();
        return true;
    }
    return false;
}

#pragma endregion Helper Funcs