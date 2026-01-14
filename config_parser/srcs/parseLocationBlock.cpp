#include "ConfigParser.hpp"

//---------------------------------------------------------------------------//
//						  PARSE LOCATION BLOCK
//---------------------------------------------------------------------------//

/**
 * @brief Parses all directives handled by a location block and stores them
 * in the relevant variables of the `LocationBlock` object. Handles syntax errors,
 * invalid directives, missing required values, etc.
 * @attention Anything that is a path is not validated during parsing!
 * This must be done during execution.
 */
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
//								RETURN
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//							HTTP METHODS
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//								MAX SIZE
//---------------------------------------------------------------------------//

/**
 * @brief Grabs and bound checks value associated with `max_size` directive
 * @attention Accepts and handles `G` as unit specifier, despite max_size
 * being limited to 100M
 * @note See `PARSING HELPERS - TOKEN HANDLING HELPERS` section in ConfigParser.cpp
 * for the definitions of `getSizeAndUnit()`
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


//---------------------------------------------------------------------------//
//								AUTO INDEX
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//							 ERROR PAGES
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//								 INDEX
//---------------------------------------------------------------------------//

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


//---------------------------------------------------------------------------//
//								  ROOT
//---------------------------------------------------------------------------//

void	ConfigParser::parseRoot(LocationBlock& l) {
	Token	rootPath = expect(TOKEN_WORD, "expected path for root:");

	l.root = rootPath.value;
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


