/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:34:08 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 16:15:49 by ameechan         ###   ########.fr       */
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

class ConfigParser {
	private:
		const std::vector<Token>&	tokens;
		size_t						currentIndex;

		Token	peek() const;
		Token	peekNext() const;
		Token	consume();
		Token	expect(TokenType type, const std::string& msg);

		bool	isAtEnd() const;
		bool	match(TokenType type);
		bool	check(TokenType type) const;
		bool	matchWord(const std::string& value);
		bool	checkWord(const std::string& value) const;

		ServerBlock	parseServerBlock();
		// void parseLocationBlock(ServerBlock& server);
		// ... other parsing methods
	public:
		ConfigParser(const std::vector<Token>& toks);
		~ConfigParser();
		void	parse(Config& data); // May throw ParseException
};

#endif