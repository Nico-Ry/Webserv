/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:47 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 17:49:19 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "ServerBlock.hpp"

typedef std::vector<std::string> StringVec;
class ServerBlock;

class LocationBlock {
	public:
		LocationBlock(const ServerBlock& s);
		~LocationBlock();

		std::string					uri;
		std::vector<std::string>	methods;

		std::string					root;
		std::vector<std::string>	index;
		bool						autoIndex;
		std::map<int, StringVec>	errorPages;
		size_t						clientMaxBodySize;

		bool						hasRedirect;
		int							redirectCode;
		std::string					redirectTarget;
};

#endif