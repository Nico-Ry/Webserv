/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:31 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 17:53:41 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationBlock.hpp"

LocationBlock::LocationBlock(const ServerBlock& s) :
	root(s.root),
	index(s.index),
	autoIndex(s.autoIndex),
	clientMaxBodySize(s.clientMaxBodySize),
	hasRedirect(false),
	redirectCode(0)//	Set to Zero by default
	{}

LocationBlock::~LocationBlock() {}