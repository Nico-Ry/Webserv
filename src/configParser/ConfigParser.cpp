#include "configParser/ConfigParser.hpp"
#include "utils.hpp"


//---------------------------------------------------------------------------//
//						   CONSTRUCTOR & DESTRUCTOR
//---------------------------------------------------------------------------//

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
	serverDirectives["upload"] = &ConfigParser::parseUpload;

//build map for Location directives(KEY) to function pointers(VALUE)
	locationDirectives["root"] = &ConfigParser::parseRoot;
	locationDirectives["index"] = &ConfigParser::parseIndex;
	locationDirectives["error_page"] = &ConfigParser::parseErrorPages;
	locationDirectives["autoindex"] = &ConfigParser::parseAutoIndex;
	locationDirectives["max_size"] = &ConfigParser::parseMaxSize;
	locationDirectives["methods"] = &ConfigParser::parseMethods;
	locationDirectives["upload"] = &ConfigParser::parseUpload;
	locationDirectives["return"] = &ConfigParser::parseReturn;
	locationDirectives["cgi_bin"] = &ConfigParser::parseCgiBin;
	locationDirectives["cgi_extension"] = &ConfigParser::parseCgiExtension;

}

ConfigParser::~ConfigParser() {}



//---------------------------------------------------------------------------//
//									PARSE
//---------------------------------------------------------------------------//

/**
 * @brief Starts the whole parsing process, building ServerBlocks
 * 1 by 1 and adding them progressively to `data.server`
 * @param data The main Config object
 * @note `data` has a member `servers` which is a vector storing all the
 * ServerBlocks that were built during parsing
 */
void	ConfigParser::parse(Config& data)
{

	while (!isAtEnd())
	{
		data.servers.push_back(parseServerBlock());
	}
	//[DEBUG] output to see all parsed/stored values
	printAllOutput(data);
}


//###########################################################################//
//				   PARSING HELPERS - TOKEN HANDLING HELPERS
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//

//---------------------------------------------------------------------------//
//							 GET SIZE AND UNIT
//---------------------------------------------------------------------------//

/**
 * @brief updates `unit` and consumes the current token if peek().value
 * is an accepted unit specifier (K, M or G)
 */
void	ConfigParser::updateUnit(std::string& unit, const std::string& currentToken)
{
	if (currentToken == "K" || currentToken == "M" || currentToken == "G")
		unit = consume().value;
}

/**
 * @brief Parses `sizeToken` grabbing the numeric value and any trailing unit specifiers.
 * Also handles cases where the unit specifier is stored in the following Token
 * @attention `parseMaxSize()` consumes `sizeToken`, thus when this function is called,
 * the current Token points to the token following `sizeToken`
 * @return throws error on invalid value and/or unit specifier
 */
void	ConfigParser::getSizeAndUnit(const Token& sizeToken, long& num, std::string& unit)
{
	std::stringstream	ss(sizeToken.value);
	std::string			unit_err = "max_size: invalid unit specifier, expected K, M or G: ";

	if (!(ss >> num))
		throw ParseException("max_size invalid input:", sizeToken);

	if (num < 0)
		throw ParseException("max_size negative number:", sizeToken);

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
		throw ParseException("max_size invalid unit", sizeToken.line);
	if (unit != "K" && unit != "M" && unit != "G")
		throw ParseException("max_size invalid unit", sizeToken.line);
}


//---------------------------------------------------------------------------//
//							   IS DIRECTIVE
//---------------------------------------------------------------------------//

bool	ConfigParser::isDirective(const std::string& value)
{
// look through map for matching directive
	std::map<std::string, ServerFn>::iterator it;
	it = serverDirectives.find(value);

// If directive not found in map, return false
	if (it == serverDirectives.end())
		return (false);
	return (true);
}


//---------------------------------------------------------------------------//
//									PEEK
//---------------------------------------------------------------------------//

//Looks at current Token without consuming it
Token	ConfigParser::peek() const
{
	if (currentIndex < tokens.size())
		return (tokens[currentIndex]);
	return (tokens.back());
}


//---------------------------------------------------------------------------//
//								 PEEK NEXT
//---------------------------------------------------------------------------//

//Looks at next Token without consuming it
Token	ConfigParser::peekNext() const
{
	if (currentIndex + 1 < tokens.size())
		return (tokens[currentIndex + 1]);
	return (tokens.back());
}


//---------------------------------------------------------------------------//
//								  CONSUME
//---------------------------------------------------------------------------//

/**
 * @brief Consumes and returns current token
 */
Token	ConfigParser::consume()
{
	if (isAtEnd())
		throw(ParseException("Can't consume past end of tokens!", peek().line));
	Token	current = peek();
	++currentIndex;
	return (current);
}


//---------------------------------------------------------------------------//
//								  EXPECT
//---------------------------------------------------------------------------//

/**
 * @brief expects a specific token, throws error if not expected type
 * @attention Consumes token if types match
 * @return Current token if types match, throws error if no match
 */
Token	ConfigParser::expect(TokenType type, const std::string& msg)
{
	// Token	current = peek();
	if (!check(type))
		throw ParseException(msg, peek().line);
	return (consume());
}


//---------------------------------------------------------------------------//
//								IS AT END
//---------------------------------------------------------------------------//

bool	ConfigParser::isAtEnd() const
{
	if (currentIndex >= tokens.size())
		return (true);
	return (false);
}


//---------------------------------------------------------------------------//
//								  CHECK
//---------------------------------------------------------------------------//

/**
 * @brief Checks if current token's type matches with `type`
 * @return `true` if types match, `false` otherwise
 */
bool	ConfigParser::check(TokenType type) const
{
	if (peek().type == type)
		return (true);
	return (false);
}


//---------------------------------------------------------------------------//
//								  MATCH
//---------------------------------------------------------------------------//

/**
 * @brief Checks if current token's type matches with `type` and consumes
 * the current token if true
 * @attention Consumes token on true
 * @return `true` if token type matches, `false` otherwise
 */
bool	ConfigParser::match(TokenType type)
{
	if (check(type))
	{
		consume();
		return (true);
	}
	return (false);
}


//---------------------------------------------------------------------------//
//							   CHECK WORD
//---------------------------------------------------------------------------//

/**
 * @brief Checks that current token is TOKEN_WORD and that its
 * value matches `value`
 * @return `true` if word and matches value, `false` otherwise
 */
bool ConfigParser::checkWord(const std::string& value) const
{
	Token current = peek();
	return (current.type == TOKEN_WORD && current.value == value);
}


//---------------------------------------------------------------------------//
//							   MATCH WORD
//---------------------------------------------------------------------------//

/**
 * @brief Matches current token's value with `value` and consumes
 * the token if both values match
 * @attention Consumes token on true
 * @return `true` if values match, `false` otherwise
 */
bool ConfigParser::matchWord(const std::string& value)
{
	if (checkWord(value))
	{
		consume();
		return (true);
	}
	return (false);
}
