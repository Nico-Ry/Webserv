#include "configParser/ConfigParser.hpp"

//---------------------------------------------------------------------------//
//							  PARSE SERVER BLOCK
//---------------------------------------------------------------------------//

/**
 * @brief Parses all directives handled by a server block and stores them
 * in the relevant variables of `ServerBlock` object. Handles syntax and
 * semantic errors, including: invalid directives, missing required values,
 * invalid ports, etc.
 * @attention Anything that is a path is not validated during parsing!
 * This must be done during execution.
 */
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

//---------------------------------------------------------------------------//
//									MAX SIZE
//---------------------------------------------------------------------------//

/**
 * @brief Grabs and bound checks value associated with `max_size` directive
 * @attention Accepts and handles `G` as unit specifier, despite max_size
 * being limited to 100M
 * @note See `PARSING HELPERS - TOKEN HANDLING HELPERS` section in ConfigParser.cpp
 * for the definitions of `getSizeAndUnit()`
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



//---------------------------------------------------------------------------//
//								AUTO INDEX
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//								ERROR PAGES
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//								  INDEX
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//								  LISTEN
//---------------------------------------------------------------------------//

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
	s.hasPort = true;
}


//---------------------------------------------------------------------------//
//								  ROOT
//---------------------------------------------------------------------------//

void	ConfigParser::parseRoot(ServerBlock& s) {
	Token	rootPath = expect(TOKEN_WORD, "expected path for root:");

	s.root = rootPath.value;
	expect(TOKEN_SEMICOLON, "Expected ';'");
	s.hasRoot = true;
}

// #pragma endregion PARSE SERVER DIRECTIVES

