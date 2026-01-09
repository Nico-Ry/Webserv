/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:43 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 20:25:09 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationBlock.hpp"

struct CommonBlock;

class ServerBlock : public CommonBlock {
	public:
		ServerBlock();
		~ServerBlock();

		int							port;
		std::vector<std::string>	defaultMethods;
		std::vector<LocationBlock>	locations;
		// bool						hasRedirect;
		// int						redirectCode;
		// std::string				redirectTarget;
};

#endif