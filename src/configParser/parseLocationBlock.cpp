#include "configParser/ConfigParser.hpp"

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
void		ConfigParser::parseLocationBlock(ServerBlock& s)
{
	Token	uri = expect(TOKEN_WORD, "Expected <URI>");
	LocationBlock	newBlock(s);
	newBlock.uri = uri.value;

	expect(TOKEN_LBRACE, "Expected '{'");
	while (!check(TOKEN_RBRACE))
	{
		Token	directive = expect(TOKEN_WORD, "Expected directive");

	// Find pointer to parsing function that matches directive.value
		std::map<std::string, LocationFn>::iterator	it =
			locationDirectives.find(directive.value);

	// If directive not found in map, throw error
		if (it == locationDirectives.end())
			throw ParseException("Unkown directive: ", directive);

	// Else call function pointer to parse directive
		(this->*(it->second))(newBlock);
	}
	expect(TOKEN_RBRACE, "Expected '}'");
	s.locations.push_back(newBlock);
}

//---------------------------------------------------------------------------//
//								CGI
//---------------------------------------------------------------------------//
void	ConfigParser::parseCgiBin(LocationBlock& l)
{
	Token cgiBinToken = expect(TOKEN_WORD, "expected path for cgi-bin:");
	std::string path = cgiBinToken.value;

	if (path.empty())
		throw ParseException("CGI Bin cannot be empty", cgiBinToken.line);

	l.cgiBin = path;
	l.hasCgiBin = true;

	expect(TOKEN_SEMICOLON, "Expected ';'");
}

void	ConfigParser::parseCgiExtension(LocationBlock& l)
{
	Token cgiExtToken = expect(TOKEN_WORD, "expected CGI extension (e.g. .py):");
	std::string extension = cgiExtToken.value;

	if (extension.empty())
		throw ParseException("CGI Extension cannot be empty", cgiExtToken.line);
	if (extension[0] != '.')
		throw ParseException("CGI Extension must start with a dot:", cgiExtToken.line);
	if (extension.size() == 1)
		throw ParseException("invalid CGI Extension", cgiExtToken);

	l.cgiExtension = extension;
	l.hasCgiExtension = true;

	expect(TOKEN_SEMICOLON, "Expected ';'");
}

//---------------------------------------------------------------------------//
//								RETURN
//---------------------------------------------------------------------------//

bool	ConfigParser::isValidRedirectCode(const int& code)
{
	if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308)
		return (true);
	return (false);
}


void	ConfigParser::parseReturn(LocationBlock& l)
{
	// Make sure current token is NOT a special character
	if (!check(TOKEN_WORD))
		expect(TOKEN_WORD, "Expected HTTP status code");

	std::stringstream	ss(peek().value);
	int					statusCode;

//		not an int	||	trailing non-digits	||	invalid redirect status code
	if (!(ss >> statusCode) || !ss.eof() || !isValidRedirectCode(statusCode))
		throw ParseException("invalid redirect status code:", peek());
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

bool	ConfigParser::isMethod(const std::string& value)
{
	if (value == "GET" || value == "POST" || value == "DELETE")
		return (true);
	return (false);
}


void	ConfigParser::parseMethods(LocationBlock& l)
{
	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected method identifier");

	while (true)
	{
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD)) {
			if (isDirective(peek().value))
				expect(TOKEN_SEMICOLON, "Expected ';'");
			if (isMethod(peek().value))
				l.methods.push_back(consume().value);
			else
				throw ParseException("Unknown method:", peek());
		}
	}
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


//---------------------------------------------------------------------------//
//									 UPLOAD
//---------------------------------------------------------------------------//

void	ConfigParser::parseUpload(LocationBlock& l) {
	Token	uploadToken = expect(TOKEN_WORD, "expected path for upload directory:");
	std::string	path = uploadToken.value;

	if (path.empty())
		throw ParseException("Upload Directory cannot be empty", uploadToken.line);

	if (path[0] == '/')
	{
		std::stringstream	ss;
		ss	<< CYAN << "line" << std::setw(4) << uploadToken.line
			<< RES << "| "
			<< BOLD_ORANGE << "Warning:"
			<< RES << " upload directory may be misconfigured: "
			<< ORANGE << path << RES << std::endl;
		std::cerr << ss.str();
	}

	l.uploadDir = path;
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
void	ConfigParser::parseMaxSize(LocationBlock& l)
{
	long		num;
	std::string	unit;
	Token		sizeToken = expect(TOKEN_WORD, "Expected size specifier");

	getSizeAndUnit(sizeToken, num, unit);
	expect(TOKEN_SEMICOLON, "Expected ';'");

	size_t		maxSize = 0;
	if (unit == "K")
		maxSize = static_cast<size_t>(num) * 1024UL;
	else if (unit == "M")
		maxSize = static_cast<size_t>(num) * 1024UL * 1024UL;
	else if (unit == "G")
		maxSize = static_cast<size_t>(num) * 1024UL * 1024UL * 1024UL;

	if (maxSize > 104857600) // max size 100M
		throw ParseException("size too large (max 100M):", sizeToken);
	l.clientMaxBodySize = maxSize;
}


//---------------------------------------------------------------------------//
//								AUTO INDEX
//---------------------------------------------------------------------------//

void	ConfigParser::parseAutoIndex(LocationBlock& l)
{
	if (peek().value == "on")
		l.autoIndex = true;
	else if (peek().value == "off")
		l.autoIndex = false;
	else
		throw ParseException("Unknown boolean:", peek());

	consume(); //consume on/off
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


//---------------------------------------------------------------------------//
//							 ERROR PAGES
//---------------------------------------------------------------------------//

void	ConfigParser::parseErrorPages(LocationBlock& l)
{
	std::stringstream	ss(peek().value);
	long				err_code;
	StringVec			err_pages;

	if (!(ss >> err_code))
		throw ParseException("not an error code or out of range:", peek());
	if (!ss.eof())
		throw ParseException("invalid error code input:", peek());
	if (err_code < 400 || err_code > 599)
		throw ParseException("inadequate error code:", peek());

	consume(); // consume error code
	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected error file");

	while (true)
	{
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD))
		{
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

void	ConfigParser::parseIndex(LocationBlock& l)
{

	if (!check(TOKEN_WORD)) // only consume via expect if type != word
		expect(TOKEN_WORD, "Expected index file");

	while (true)
	{
		if (check(TOKEN_SEMICOLON) || check(TOKEN_RBRACE))
			break;
		if (check(TOKEN_WORD))
		{
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

void	ConfigParser::parseRoot(LocationBlock& l)
{
	Token	rootToken = expect(TOKEN_WORD, "expected path for root:");
	std::string	root = rootToken.value;

	if (root.empty())
		throw ParseException("root cannot be empty", rootToken.line);

	if (root[0] == '/')
	{
		std::stringstream	ss;
		ss	<< CYAN << "line" << std::setw(4) << rootToken.line
			<< RES << "| "
			<< BOLD_ORANGE << "Warning:"
			<< RES << " root may be misconfigured: "
			<< ORANGE << root << RES << std::endl;
		std::cerr << ss.str();
	}

	l.root = root;
	expect(TOKEN_SEMICOLON, "Expected ';'");
}


