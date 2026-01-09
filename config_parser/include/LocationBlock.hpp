/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:47 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 21:13:07 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>
#include <map>

class LocationBlock {
	public:
		LocationBlock();
		~LocationBlock();

		std::string					uri;
		std::vector<std::string>	methods;

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