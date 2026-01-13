/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 10:45:45 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 10:46:15 by ameechan         ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include "ConfigParser.hpp"
#include "Tokeniser.hpp"

/**
 * @brief The main object that contains all ServerBlocks, which in turn
 * contain all LocationBlocks within them
 */
class Config {
	public:
//				CONSTRUCTOR & DESTRUCTOR
		Config(const std::string& configFile);
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

// 		vector of all ServerBlocks defined in config file
		std::vector<ServerBlock>	servers;
};

#endif