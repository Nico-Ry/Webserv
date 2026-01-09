/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:43 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 21:13:03 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationBlock.hpp"

struct CommonBlock;

class ServerBlock {
	public:
		ServerBlock();
		~ServerBlock();

		int							port;
		std::vector<std::string>	defaultMethods;
		std::vector<LocationBlock>	locations;

		std::string					root;
		std::vector<std::string>	index;
		bool						autoIndex;
		std::map<int, std::string>	errorPages;
		// size_t					clientMaxBodySize;

		// bool						hasRedirect;
		// int						redirectCode;
		// std::string				redirectTarget;
};

#endif