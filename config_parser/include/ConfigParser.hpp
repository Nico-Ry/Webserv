/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:34:08 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 18:23:34 by ameechan         ###   ########.fr       */
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


class ConfigParser {
	private:
		const std::vector<Token>&			tokens;
		size_t								currentIndex;

	// Directive to function pointer map
	// Each directive keyword is paired with
	// a pointer to its respective parsing function
		std::map<std::string, ServerFn>		serverDirectives;//		ServerBlocks
		std::map<std::string, LocationFn>	locationDirectives;//	LocationBlocks

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
		bool	isDirective(const std::string& value);

//			SERVER BLOCK PARSING FUNCTIONS
		ServerBlock	parseServerBlock();
		void		parseLocationBlock(ServerBlock& s);
		void		parseListen(ServerBlock& s);
		void		parseRoot(ServerBlock& s);
		void		parseIndex(ServerBlock& s);
		void		parseErrorPages(ServerBlock& s);
		void		parseAutoIndex(ServerBlock& s);
		void		parseMaxSize(ServerBlock& s);
		void		getSizeAndUnit(const std::string& sizeToken, long& num, std::string& unit);
		void		updateUnit(std::string& unit, const std::string& currentToken);

//			LOCATION BLOCK PARSING FUNCTIONS
		void		parseRoot(LocationBlock& l);
		void		parseIndex(LocationBlock& l);
		void		parseErrorPages(LocationBlock& l);
		void		parseAutoIndex(LocationBlock& l);
		void		parseMaxSize(LocationBlock& l);
		void		parseMethods(LocationBlock& l);
		bool		isMethod(const std::string& value);
		void		parseReturn(LocationBlock& l);
		bool		isValidRedirectCode(const int& code);


	public:
		ConfigParser(const std::vector<Token>& toks);
		~ConfigParser();
		void	parse(Config& data); // May throw ParseException
};


#endif