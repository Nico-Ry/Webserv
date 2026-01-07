/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:43 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/07 16:36:42 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationBlock.hpp"

class ServerBlock {
	private:
		std::vector<int>				ports;
		std::string						root;
		std::vector<std::string>		index;
		std::map<int, std::string>		errorPages;
		// bool							autoIndex;
		// size_t							clientMaxBodySize;
		std::vector<std::string>		serverLevelMethods;
		std::vector<LocationBlock>		locations;
	public:
		ServerBlock();
		~ServerBlock();
};

#endif