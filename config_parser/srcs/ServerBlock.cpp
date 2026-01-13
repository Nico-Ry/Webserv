/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:27 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 18:51:41 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBlock.hpp"

/**
 * @brief Default Constructor - Builds a ServerBlock object setting default values
 * for `port`, `autoIndex` and `clientMaxBodySize`
 */
ServerBlock::ServerBlock()
	: port(0), autoIndex(false), clientMaxBodySize(512 * 1024UL) {
	defaultMethods.push_back("GET");
}

ServerBlock::~ServerBlock() {}