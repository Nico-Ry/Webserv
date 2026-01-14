#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <istream>
#include <fstream>
#include <sstream>

//enum used to differentiate token types
typedef	enum e_token {
	TOKEN_WORD,//		0
	TOKEN_LBRACE,//		1
	TOKEN_RBRACE,//		2
	TOKEN_SEMICOLON,//	3
	TOKEN_EOF,//		4
	TOKEN_ERR//			5
}	TokenType;


/**
 * @brief Holds all useful information regarding a given token.
 * @param type the type of token as define by the enum `TokenType`
 * @param value the string containing the characters of the token
 * @param line the line at which the token appears
 * @param column the column of the first character of the token
 */
typedef struct Token {
	TokenType	type;
	std::string	value;
	size_t		line;
	size_t		column;

	//constructor that takes parameters
	Token(TokenType t, std::string v, size_t l, size_t c)
		: type(t), value(v), line(l), column(c) {}

	//default constructor
	Token() : type(TOKEN_EOF), value(""), line(0), column(0) {}
}	tok;


/**
 * @brief Reads from the given `ifstream` and creates tokens using whitespace
 * as a delimiter. Properly detects special characters `;` `{` and `}`
 * building separate tokens for them. Example: `root /www;` properly
 * results in the 3 tokens `root` `/www` and `;`
 */
class Tokeniser {
	private:
		std::ifstream&	cfgFile;
		TokenType		identifyDelimiter(const char& delimiter);
		void			pushAndFlush(std::vector<Token>& tokens, TokenType t, std::string& buf, size_t line, size_t col);

	public:
		Tokeniser(std::ifstream& fileName);
		void		tokenise(std::vector<Token>& tokens);
};



void	printTokens(const std::vector<Token>& tokens);

#endif