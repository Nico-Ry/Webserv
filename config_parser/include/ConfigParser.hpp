/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:34:08 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 20:28:12 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <sstream>
#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include "Tokeniser.hpp"
#include "Config.hpp"

struct Token;
class Config;
class ConfigParser;

typedef void	(ConfigParser::*ParseFn)(ServerBlock& s);

class ConfigParser {
	private:
		const std::vector<Token>&	tokens;
		size_t						currentIndex;
		std::map<std::string, ParseFn>	directiveHandlers;

//			HELPER FUNCTIONS
		Token	peek() const;
		Token	peekNext() const;
		Token	consume();
		Token	expect(TokenType type, const std::string& msg);
		bool	isAtEnd() const;
		bool	match(TokenType type);
		bool	check(TokenType type) const;
		bool	matchWord(const std::string& value);
		bool	checkWord(const std::string& value) const;

//			PARSING FUNCTIONS
		ServerBlock	parseServerBlock();
		// void parseLocationBlock(ServerBlock& server);
		void	parseListen(ServerBlock& s);
		// void	parseRoot(ServerBlock& s);
	public:
		ConfigParser(const std::vector<Token>& toks);
		~ConfigParser();
		void	parse(Config& data); // May throw ParseException
};

typedef void	(ConfigParser::*ParseFn)(ServerBlock& s);

#endif