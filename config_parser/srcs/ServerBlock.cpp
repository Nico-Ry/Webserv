/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:27 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/08 18:19:36 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBlock.hpp"

ServerBlock::ServerBlock() {}
ServerBlock::~ServerBlock() {}

void	ServerBlock::setPort(const int& port) {
	this->port = port;
}

int		ServerBlock::getPort() {
	return port;
}