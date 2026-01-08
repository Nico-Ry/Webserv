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

class Config {
	private:
		std::vector<ServerBlock>	servers;
	public:
//				CONSTRUCTOR & DESTRUCTOR
		Config(const std::string& configFile);
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

//					METHODS
		// const std::vector<ServerBlock>& getServers() const;
		void		addServer(const ServerBlock& s);
		ServerBlock	getServer(const size_t index);
};

#endif