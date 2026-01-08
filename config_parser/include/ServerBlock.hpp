/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:43 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 18:19:09 by ameechan         ###   ########.fr       */
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
		int							port;
		std::string					root;
		std::vector<std::string>	index;
		std::map<int, std::string>	errorPages;
		// bool						autoIndex;
		// size_t					clientMaxBodySize;
		std::vector<std::string>	serverLevelMethods;
		std::vector<LocationBlock>	locations;

	public:
		ServerBlock();
		~ServerBlock();
		void	setPort(const int& port);
		int		getPort();
};

#endif