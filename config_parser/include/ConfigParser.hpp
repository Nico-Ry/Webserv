/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 11:34:08 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 12:03:22 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include "Config.hpp"

struct Token;
class Config;

class ConfigParser {
	private:
		std::vector<Token>	tokens;
		size_t				currentIndex;

	public:
		ConfigParser(const std::vector<Token>& toks);
		~ConfigParser();
		void	parse(Config& data);
};

#endif