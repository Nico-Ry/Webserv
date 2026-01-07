/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:47 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/07 16:37:01 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>
#include <map>

class LocationBlock {
	private:
		std::string					uri;
		std::string					Root;
		std::vector<std::string>	Index;
		std::vector<std::string>	localLevelMethods;
		// bool						autoIndex;
		std::map<int, std::string>	errorPages;
		// size_t						clientMaxBodySize;
		// bool						hasRedirect;
		// std::string					redirectTarget;
		// int							redirectCode;
	public:
		LocationBlock();
		~LocationBlock();
};

#endif