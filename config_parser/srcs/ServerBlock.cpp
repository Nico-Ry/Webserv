/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:27 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 21:13:42 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBlock.hpp"

ServerBlock::ServerBlock() : port(0), autoIndex(false) {
	defaultMethods.push_back("GET");
}

ServerBlock::~ServerBlock() {}
