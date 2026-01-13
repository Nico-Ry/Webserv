/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:43 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 19:17:02 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationBlock.hpp"

typedef std::vector<std::string> StringVec;
class LocationBlock;

/**
 * @brief Class that stores all information stored between the opening
 * and closing braces of a `server` directive in the config file, including
 * any and all `location` directives contained within it.
 * @attention For any member that ServerBlock and LocationBlock have in common,
 * if the request is for a specific URI (Location), then Location always takes
 * precedence over the Server. If the member was not defined in the config for
 * the given Location, then we fallback to the Server and check the Server's info
 * @param locations All LocationBlocks objects stored within this ServerBlock
 * @param port The port the server listens on.
 * @param root Where to look in our file system (root + URI = path)
 * @param index default file to feed for GET requests
 * @param autoIndex Enables/disables directory listing if GET requested a directory
 * @param clientMaxBodySize The max size allowed for any client HTTP request
 * @param errorPages map that contains pairs of int (HTTP status code) and a vector
 * of strings (all files to try and return with the given status code)
 */
class ServerBlock {
	public:
		ServerBlock();
		~ServerBlock();

		int							port;//		Validated at parsing (not checked for reserved ports)
		std::vector<std::string>	defaultMethods;
		std::vector<LocationBlock>	locations;

		std::string					root;//		can fail gracefully (not validated)
		std::vector<std::string>	index;//	can fail gracefully (not validated)
		bool						autoIndex;
		std::map<int, StringVec>	errorPages;
		size_t						clientMaxBodySize; // defaults to 20Kb

	};

#endif