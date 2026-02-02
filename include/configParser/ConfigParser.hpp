#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <sstream>
#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include "Tokeniser.hpp"
#include "Config.hpp"
#include "http/Mime.hpp"
#include "colours.hpp"

struct Token;
class Config;
class ConfigParser;


/**
 * @brief Typedef for function pointers to any function in the
 * `ConfigParser` namespace that takes the following parameter:
 * @param ServerBlock
 */
typedef void	(ConfigParser::*ServerFn)(ServerBlock& s);

/**
 * @brief Typedef for function pointers to any function in the
 * `ConfigParser` namespace that takes the following parameter:
 * @param LocationBlock
 */
typedef void	(ConfigParser::*LocationFn)(LocationBlock& l);



/**
 * @brief Parses `tokens` building `ServerBlock` and nested `LocationBlocks`
 * as it goes. Does not check if paths or URIs are valid, as this is expected
 * to fail gracefully during execution. Contains maps `serverDirectives` and
 * `locationDirectives` that pair a given directive with the respective function
 * used to parse that directive.
 * @note a 'directive' is a protected word in config files such as:
 * 'listen' -> 'root' -> 'index' -> etc.
 */
class ConfigParser {
	private:
		const std::vector<Token>&			tokens;// vector of all tokens from config file
		size_t								currentIndex;// current token index

		// maps that pair server/location directives with their respective parsing functions
		std::map<std::string, ServerFn>		serverDirectives;
		std::map<std::string, LocationFn>	locationDirectives;



//---------------------------------------------------------------------------//
//				TOKEN HELPERS FOR CHECKING VALUES, TOKEN TYPES, ETC.
//---------------------------------------------------------------------------//

		Token	peek() const;
		Token	peekNext() const;
		Token	consume();
		Token	expect(TokenType type, const std::string& msg);
		bool	isAtEnd() const;
		bool	match(TokenType type);
		bool	check(TokenType type) const;
		bool	matchWord(const std::string& value);
		bool	checkWord(const std::string& value) const;
		bool	isDirective(const std::string& value);



//---------------------------------------------------------------------------//
//						SERVER BLOCK PARSING FUNCTIONS
//---------------------------------------------------------------------------//

		ServerBlock	parseServerBlock();
		void		parseLocationBlock(ServerBlock& s);
		void		parseListen(ServerBlock& s);
		void		parseRoot(ServerBlock& s);
		void		parseIndex(ServerBlock& s);
		void		parseErrorPages(ServerBlock& s);
		void		parseAutoIndex(ServerBlock& s);
		void		parseMaxSize(ServerBlock& s);
		void		parseUpload(ServerBlock& s);
		void		getSizeAndUnit(const Token& sizeToken, long& num, std::string& unit);
		void	parseCgiBin(LocationBlock& l);
		void	parseCgiExtension(LocationBlock& l);

		void		updateUnit(std::string& unit, const std::string& currentToken);


//---------------------------------------------------------------------------//
//					LOCATION BLOCK PARSING FUNCTIONS
//---------------------------------------------------------------------------//

		void		parseRoot(LocationBlock& l);
		void		parseIndex(LocationBlock& l);
		void		parseErrorPages(LocationBlock& l);
		void		parseAutoIndex(LocationBlock& l);
		void		parseMaxSize(LocationBlock& l);
		void		parseMethods(LocationBlock& l);
		bool		isMethod(const std::string& value);
		void		parseUpload(LocationBlock& l);
		void		parseReturn(LocationBlock& l);
		bool		isValidRedirectCode(const int& code);


	public:
		ConfigParser(const std::vector<Token>& toks);
		~ConfigParser();
		void	parse(Config& data); // May throw ParseException
};


class ParseException : public std::exception {
private:
	std::string message;
	int line;
	int column;
	std::string tokenValue;
	std::string fullMessage;  // Cached formatted message

	void buildMessage() {
		std::stringstream ss;

		// Format: "9 | Error: Root must be absolute: www/"
		ss	<< CYAN << "line" << std::setw(4) << line
			<< RES << "| "
			<< BOLD_RED << "Config file Error:"
			<< RES << " " << message;

		// Add token value if provided
		if (!tokenValue.empty()) {
			ss << " " << BOLD_RED << "'" << tokenValue << "'" << RES;
		}

		// Add column info if provided
		if (column >= 0) {
			ss << " " << YELLOW << "(column " << column << ")" << RES;
		}

		fullMessage = ss.str();
	}

public:
	// Constructor with just message
	ParseException(const std::string& msg)
		: message(msg), line(-1), column(-1), tokenValue("") {
		buildMessage();
	}

	// Constructor with line number
	ParseException(const std::string& msg, int ln)
		: message(msg), line(ln), column(-1), tokenValue("") {
		buildMessage();
	}

	// Constructor with line, column, and token
	ParseException(const std::string& msg, int ln, int col, const std::string& token = "")
		: message(msg), line(ln), column(col), tokenValue(token) {
		buildMessage();
	}

	// Constructor from Token (most convenient!)
	ParseException(const std::string& msg, const Token& token)
		: message(msg), line(token.line), column(token.column), tokenValue(token.value) {
		buildMessage();
	}

	virtual ~ParseException() throw() {}

	virtual const char* what() const throw() {
		return fullMessage.c_str();
	}

	int getLine() const { return line; }
	int getColumn() const { return column; }
	std::string getTokenValue() const { return tokenValue; }
};


#endif
