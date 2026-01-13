/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:31 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 18:48:20 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationBlock.hpp"

/**
 * @brief Builds a default LocationBlock copying defaults as defined
 * by the ServerBlock it inhabits, and defining other defaults unique
 * to LocationBlocks
 * @param root where to look in our filesystem for the given URI
 * @param index default file to feed for GET requests
 * @param autoIndex Enables/disables directory listing if GET requested a directory
 * @param clientMaxBodySize The max size allowed for any client HTTP request
 * @param hasRedirect Indicates whether a redirecction was defined
 * in the config file or not
 * @param redirectCode The HTTP status Code linked to the redirect as
 * defined in the config file. Set to 0 by default
 * @attention Note that some element such as `errorPages` are purposely
 * omitted from being copied over to the LocationBlock
 */
LocationBlock::LocationBlock(const ServerBlock& s) :
	root(s.root),
	index(s.index),
	autoIndex(s.autoIndex),
	clientMaxBodySize(s.clientMaxBodySize),
	hasRedirect(false),
	redirectCode(0)//	Set to Zero by default
	{}

LocationBlock::~LocationBlock() {}