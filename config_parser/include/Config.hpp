/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:37 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/07 18:40:11 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <sstream>
#include <istream>
#include <fstream>
#include <iostream>
#include <cctype>
#include "ServerBlock.hpp"
#include "LocationBlock.hpp"

typedef	enum e_token {
	TOKEN_WORD,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_SEMICOLON,
	TOKEN_EOF
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

class Config {
	private:
		std::vector<ServerBlock>	servers;
		// std::vector<std::string>	tokens;
		std::ifstream				cfgFile;
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();
//					MEMBER FUNCS
		bool		validInputFile(const std::string& fileName);
		void		tokeniseConfig(std::vector<Token>& tokens);
		void		parseConfigFile();
		TokenType	identifyDelimiter(const char& delimiter);
};

#endif