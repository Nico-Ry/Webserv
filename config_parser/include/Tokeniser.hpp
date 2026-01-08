/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokeniser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 13:15:59 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 13:38:39 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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


typedef	enum e_token {
	TOKEN_WORD,//		0
	TOKEN_LBRACE,//		1
	TOKEN_RBRACE,//		2
	TOKEN_SEMICOLON,//	3
	TOKEN_EOF//			4
}	TokenType;



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