/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:34:06 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 20:33:33 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "utils.hpp"

//---------------------------------------------------------------------------//
//								  CONSTRUCTORS
//---------------------------------------------------------------------------//
#pragma region CONSTRUCTORS & DESTRUCTORS
ConfigParser::ConfigParser(const std::vector<Token>& toks)
	: tokens(toks), currentIndex(0) {

//build map for server directives(KEY) to function pointers(VALUE)
	serverDirectives["listen"] = &ConfigParser::parseListen;
	serverDirectives["root"] = &ConfigParser::parseRoot;
	serverDirectives["index"] = &ConfigParser::parseIndex;
	serverDirectives["error_page"] = &ConfigParser::parseErrorPages;
	serverDirectives["autoindex"] = &ConfigParser::parseAutoIndex;
	serverDirectives["max_size"] = &ConfigParser::parseMaxSize;
	serverDirectives["location"] = &ConfigParser::parseLocationBlock;

//build map for Location directives(KEY) to function pointers(VALUE)
	locationDirectives["root"] = &ConfigParser::parseRoot;
	locationDirectives["index"] = &ConfigParser::parseIndex;
	locationDirectives["error_page"] = &ConfigParser::parseErrorPages;
	locationDirectives["autoindex"] = &ConfigParser::parseAutoIndex;
	locationDirectives["max_size"] = &ConfigParser::parseMaxSize;
	locationDirectives["methods"] = &ConfigParser::parseMethods;
	locationDirectives["return"] = &ConfigParser::parseReturn;
}

ConfigParser::~ConfigParser() {}
#pragma endregion





//---------------------------------------------------------------------------//
//						1. PARSE & PARSE SERVER BLOCK
//---------------------------------------------------------------------------//
#pragma region PARSE SERVER BLOCKS
/**
 * @brief Starts the whole parsing process, building ServerBlocks
 * 1 by 1 and adding them progressively to `data.server`
 * @param data The main Config object
 * @note `data` has a member `servers` which is a vector storing all the
 * ServerBlocks that were built during parsing
 */
void	ConfigParser::parse(Config& data) {

	while (!isAtEnd()) {
		data.servers.push_back(parseServerBlock());
	}
	printServerPorts(data);
	std::cout << "-------------------------" << std::endl;
	printServerRoot(data);
	std::cout << "-------------------------" << std::endl;
	printServerIndex(data);
	std::cout << "-------------------------" << std::endl;
	printServerErrorPages(data);
	std::cout << "-------------------------" << std::endl;
	printServerAutoIndex(data);
	std::cout << "-------------------------" << std::endl;
	printServerMaxSize(data);
	std::cout << "-------------------------" << std::endl;
	for (size_t i=0; i < data.servers.size(); ++i) {
		ServerBlock	current = data.servers[i];
		std::cout << "~SERVER " << i+1 << "~" << std::endl;
		printLocationRoot(current);
		std::cout << std::endl;
		printLocationIndex(current);
		std::cout << std::endl;
		printLocationErrorPages(current);
		std::cout << std::endl;
		printLocationAutoIndex(current);
		std::cout << std::endl;
		printLocationMaxSize(current);
		std::cout << std::endl;
		printLocationMethods(current);
		std::cout << std::endl;
		printLocationRedirect(current);
	std::cout << "\n-------------------------" << std::endl;
	}
}


ServerBlock	ConfigParser::parseServerBlock() {
	ServerBlock	s;

	if (!matchWord("server"))// Expect word "server"
		throw std::runtime_error("Unkown directive: " + peek().value);

	expect(TOKEN_LBRACE, "Expected '{'");
	while (!check(TOKEN_RBRACE)) {
		Token	directive = expect(TOKEN_WORD, "Expected directive");

	// Find pointer to parsing function that matches directive.value
		std::map<std::string, ServerFn>::iterator	it =
			serverDirectives.find(directive.value);

	// If directive not found in map, throw error
		if (it == serverDirectives.end())
			throw std::runtime_error("Unkown directive: " + directive.value);

	// Else call function pointer to parse directive
		(this->*(it->second))(s);
	}
	expect(TOKEN_RBRACE, "Expected '}'");
	return s;
}
#pragma endregion


//---------------------------------------------------------------------------//
//				 2. PARSING ALL THE DIRECTIVES FOR SERVER BLOCKS
//---------------------------------------------------------------------------//
#pragma region PARSE DIRECTIVES FOR SERVER BLOCKS

/**
 * @brief Parses `sizeToken` grabbing the numeric value and any trailing unit specifiers.
 * Also handles cases where the unit specifier is stored in the following Token
 * @attention `parseMaxSize()` consumes `sizeToken`, thus when this function is called,
 * the current Token points to the token following `sizeToken`
 * @return throws error on invalid value and/or unit specifier
 */
void	ConfigParser::getSizeAndUnit(const std::string& sizeToken, long& num, std::string& unit) {
	std::stringstream	ss(sizeToken);
	std::string			unit_err = "max_size: invalid unit specifier, expected K, M or G: ";

	if (!(ss >> num))
		throw std::runtime_error("max_size: invalid input: " + sizeToken);

	if (num < 0)
		throw std::runtime_error("max_size: negative number: " + sizeToken);

	//store remainder of ss in unit
	ss >> unit;

	// if no unit specified
	// and current Token (i.e. the token following sizeToken) is not unit specifier
	if (unit.empty() && check(TOKEN_SEMICOLON))
		unit = "M";
	// no unit specified but next Token might be unit specifier
	else if (unit.empty() && check(TOKEN_WORD))
		updateUnit(unit, peek().value);

	// check for invalid unit specifier
	if (unit.size() > 1)
		throw std::runtime_error(unit_err + sizeToken);
	if (unit != "K" && unit != "M" && unit != "G")
		throw std::runtime_error(unit_err + sizeToken);
}

/**
 * @brief updates `unit` and consumes the current token if peek().value
 * is an accepted unit specifier (K, M or G)
 */
void	ConfigParser::updateUnit(std::string& unit, const std::string& currentToken) {
	if (currentToken == "K" || currentToken == "M" || currentToken == "G")
		unit = consume().value;
}

/**
 * @brief Grabs and bound checks value associated with `max_size` directive
 * @attention Accepts and handles `G` as unit specifier, despite max_size
 * being limited to 100M
 */
void	ConfigParser::parseMaxSize(ServerBlock& s) {
	long		num;
	std::string	unit;
	Token		sizeToken = expect(TOKEN_WORD, "Expected size specifier: ");

	getSizeAndUnit(sizeToken.value, num, unit);
	expect(TOKEN_SEMICOLON, "Expected ';'");

	size_t		maxSize = 0;
	if (unit == "K")
		maxSize = static_cast<size_t>(num) * 1024UL;
	else if (unit == "M")
		maxSize = static_cast<size_t>(num) * 1024UL * 1024UL;
	else if (unit == "G")
		maxSize = static_cast<size_t>(num) * 1024UL * 1024UL * 1024UL;

	if (maxSize > 104857600) // max size 100M
		throw std::runtime_error("max_size: too large (max 100M): " + sizeToken.value);
	s.clientMaxBodySize = maxSize;
}


void	ConfigParser::parseAutoIndex(ServerBlock& s) {
	if (peek().value == "on")
		s.autoIndex = true;
	else if (peek().value == "off")
		s.autoIndex = false;
	else
		throw std::runtime_error("Unknown boolean: " + peek().value);

	consume(); //consume on/off
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


bool	ConfigParser::isDirective(const std::string& value) {
// look through map for matching directive
	std::map<std::string, ServerFn>::iterator it;
	it = serverDirectives.find(value);

// If directive not found in map, return false
	if (it == serverDirectives.end())
		return false;
	return true;
}


void	ConfigParser::parseErrorPages(ServerBlock& s) {
	std::stringstream	ss(peek().value);
	long				err_code;
	StringVec			err_pages;

	if (!(ss >> err_code)) {
		std::cerr << peek().line << " |"; //[DEBUG]
		throw std::runtime_error("error_page: not an error code or out of range: " + peek().value);
	}
	if (!ss.eof())
		throw std::runtime_error("error_page: invalid error code input: " + peek().value);
	if (err_code < 400 || err_code > 599)
		throw std::runtime_error("error_page: inadequate error code: " + peek().value);

	consume(); // consume error code
	if (!check(TOKEN_WORD)) // expect at least one error file/path
		expect(TOKEN_WORD, "Expected error file");

	while (true) {
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD)) {
			if (isDirective(peek().value))
				expect(TOKEN_SEMICOLON, "Expected ';'"); // missing semicolon

			err_pages.push_back(consume().value);
		}
	}
	expect(TOKEN_SEMICOLON, "Expected ';'");
	// s.errorPages[err_code] = err_pages;
	s.errorPages.insert(std::pair<int, StringVec>(err_code, err_pages)); //err_code] = err_pages;
}

void	ConfigParser::parseIndex(ServerBlock& s) {

	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected index file");

	while (true) {
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD)) {
			if (isDirective(peek().value))
				expect(TOKEN_SEMICOLON, "Expected ';'");
			s.index.push_back(consume().value);
			}
	}
	expect(TOKEN_SEMICOLON, "Expected ';'");
}

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
	Token	rootPath = expect(TOKEN_WORD, "expected path for root:");

	s.root = rootPath.value;
	expect(TOKEN_SEMICOLON, "Expected ';'");
}

#pragma endregion PARSE SERVER DIRECTIVES



//---------------------------------------------------------------------------//
//							3. PARSING LOCATION BLOCK
//---------------------------------------------------------------------------//

void		ConfigParser::parseLocationBlock(ServerBlock& s) {
	Token	uri = expect(TOKEN_WORD, "Expected <URI>");
	LocationBlock	newBlock(s);
	newBlock.uri = uri.value;

	expect(TOKEN_LBRACE, "Expected '{'");
	while (!check(TOKEN_RBRACE)) {
		Token	directive = expect(TOKEN_WORD, "Expected directive");

	// Find pointer to parsing function that matches directive.value
		std::map<std::string, LocationFn>::iterator	it =
			locationDirectives.find(directive.value);

	// If directive not found in map, throw error
		if (it == locationDirectives.end())
			throw std::runtime_error("Unkown directive: " + directive.value);

	// Else call function pointer to parse directive
		(this->*(it->second))(newBlock);
	}
	expect(TOKEN_RBRACE, "Expected '}'");
	s.locations.push_back(newBlock);
}


//---------------------------------------------------------------------------//
//			   4. PARSING ALL THE DIRECTIVES FOR LOCATION BLOCKS
//---------------------------------------------------------------------------//
#pragma region PARSE LOCATION DIRECTIVES

bool	ConfigParser::isValidRedirectCode(const int& code) {
	if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308)
		return true;
	return false;
}

void	ConfigParser::parseReturn(LocationBlock& l) {
	// Make sure current token is NOT a special character
	if (!check(TOKEN_WORD))
		expect(TOKEN_WORD, "Expected HTTP status code");

	std::stringstream	ss(peek().value);
	int					statusCode;

//		not an int	||	trailing non-digits	||	invalid redirect status code
	if (!(ss >> statusCode) || !ss.eof() || !isValidRedirectCode(statusCode)) {
		std::cerr << peek().line << " |"; //[DEBUG]
		throw std::runtime_error("error_page: invalid redirect status code: " + peek().value);
	}
	consume();// Consume HTTP status code

	// Expect URI for redirect followed by semicolon
	Token	uri = expect(TOKEN_WORD, "Expected URI for redirect");
	expect(TOKEN_SEMICOLON, "Expected ';'");

	// Valid -> Store all data in LocationBlock
	l.hasRedirect = true;
	l.redirectCode = statusCode;
	l.redirectTarget = uri.value;
}

bool	ConfigParser::isMethod(const std::string& value) {
	if (value == "GET" || value == "POST" || value == "DELETE")
		return true;
	return false;
}

void	ConfigParser::parseMethods(LocationBlock& l) {
	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected method identifier");

	while (true) {
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD)) {
			if (isDirective(peek().value))
				expect(TOKEN_SEMICOLON, "Expected ';'");
			if (isMethod(peek().value))
				l.methods.push_back(consume().value);
			else
				throw std::runtime_error("Unknown method: " + peek().value);
		}
	}
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


/**
 * @brief Grabs and bound checks value associated with `max_size` directive
 * @attention Accepts and handles `G` as unit specifier, despite max_size
 * being limited to 100M
 */
void	ConfigParser::parseMaxSize(LocationBlock& l) {
	long		num;
	std::string	unit;
	Token		sizeToken = expect(TOKEN_WORD, "Expected size specifier: ");

	getSizeAndUnit(sizeToken.value, num, unit);
	expect(TOKEN_SEMICOLON, "Expected ';'");

	size_t		maxSize = 0;
	if (unit == "K")
		maxSize = static_cast<size_t>(num) * 1024UL;
	else if (unit == "M")
		maxSize = static_cast<size_t>(num) * 1024UL * 1024UL;
	else if (unit == "G")
		maxSize = static_cast<size_t>(num) * 1024UL * 1024UL * 1024UL;

	if (maxSize > 104857600) // max size 100M
		throw std::runtime_error("max_size: too large (max 100M): " + sizeToken.value);
	l.clientMaxBodySize = maxSize;
}


void	ConfigParser::parseAutoIndex(LocationBlock& l) {
	if (peek().value == "on")
		l.autoIndex = true;
	else if (peek().value == "off")
		l.autoIndex = false;
	else
		throw std::runtime_error("Unknown boolean: " + peek().value);

	consume(); //consume on/off
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


void	ConfigParser::parseErrorPages(LocationBlock& l) {
	std::stringstream	ss(peek().value);
	long				err_code;
	StringVec			err_pages;

	if (!(ss >> err_code)) {
		std::cerr << peek().line << " |"; //[DEBUG]
		throw std::runtime_error("error_page: not an error code or out of range: " + peek().value);
	}
	if (!ss.eof())
		throw std::runtime_error("error_page: invalid error code input: " + peek().value);
	if (err_code < 400 || err_code > 599)
		throw std::runtime_error("error_page: inadequate error code: " + peek().value);

	consume(); // consume error code
	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected error file");

	while (true) {
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD)) {
			if (isDirective(peek().value))
				expect(TOKEN_SEMICOLON, "Expected ';'"); // missing semicolon

			err_pages.push_back(consume().value);
		}
	}
	expect(TOKEN_SEMICOLON, "Expected ';'");
	// s.errorPages[err_code] = err_pages;
	l.errorPages.insert(std::pair<int, StringVec>(err_code, err_pages)); //err_code] = err_pages;
}



void	ConfigParser::parseIndex(LocationBlock& l) {

	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected index file");

	while (true) {
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD)) {
			if (isDirective(peek().value))
				expect(TOKEN_SEMICOLON, "Expected ';'");
			l.index.push_back(consume().value);
			}
	}
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


void	ConfigParser::parseRoot(LocationBlock& l) {
	Token	rootPath = expect(TOKEN_WORD, "expected path for root:");

	l.root = rootPath.value;
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


#pragma endregion PARSE LOCATION DIRECTIVES




//---------------------------------------------------------------------------//
//			5. TOKEN HELPERS FOR CHECKING VALUES, TOKEN TYPES, ETC.
//---------------------------------------------------------------------------//
#pragma region TOKEN HELPERS -> Peek, expect, etc.

//Looks at current Token without consuming it
Token	ConfigParser::peek() const {
	if (currentIndex < tokens.size())
		return tokens[currentIndex];
	return tokens.back();
}

//Looks at next Token without consuming it
Token	ConfigParser::peekNext() const {
	if (currentIndex + 1 < tokens.size())
		return tokens[currentIndex + 1];
	return tokens.back();
}


/**
 * @brief Consumes and returns current token
 */
Token	ConfigParser::consume() {
	if (isAtEnd())
		throw(std::runtime_error("Can't consume past end of tokens!"));
	Token	current = peek();
	++currentIndex;
	return current;
}

/**
 * @brief expects a specific token, throws error if not expected type
 * @attention Consumes token if types match
 * @return Current token if types match, throws error if no match
 */
Token	ConfigParser::expect(TokenType type, const std::string& msg) {
	// Token	current = peek();
	if (!check(type)) {
		std::stringstream	ss;
		ss << "Parse Error at line " << peek().line
			<< ", column " << peek().column
			<< ": " << msg
			<< " (got '" << peek().value << "' instead)";
		throw std::runtime_error(ss.str());
	}
	return consume();
}

bool	ConfigParser::isAtEnd() const {
	if (currentIndex >= tokens.size())
		return true;
	return false;
}


/**
 * @brief Checks if current token's type matches with `type`
 * @return `true` if types match, `false` otherwise
 */
bool	ConfigParser::check(TokenType type) const {
	if (peek().type == type)
		return true;
	return false;
}

/**
 * @brief Checks if current token's type matches with `type` and consumes
 * the current token if true
 * @attention Consumes token on true
 * @return `true` if token type matches, `false` otherwise
 */
bool	ConfigParser::match(TokenType type) {
	if (check(type)) {
		consume();
		return true;
	}
	return false;
}


/**
 * @brief Checks that current token is TOKEN_WORD and that its
 * value matches `value`
 * @return `true` if word and matches value, `false` otherwise
 */
bool ConfigParser::checkWord(const std::string& value) const {
    Token current = peek();
    return current.type == TOKEN_WORD && current.value == value;
}

/**
 * @brief Matches current token's value with `value` and consumes
 * the token if both values match
 * @attention Consumes token on true
 * @return `true` if values match, `false` otherwise
 */
bool ConfigParser::matchWord(const std::string& value) {
    if (checkWord(value)) {
        consume();
        return true;
    }
    return false;
}

#pragma endregion TOKEN HELPERS -> Peek, expect, etc.