/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 13:30:58 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 14:59:40 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

class Config;

void	printServerPorts(Config& data);
void	printServerRoot(Config& data);
void	printServerIndex(Config& data);
void	printServerErrorPages(Config& data);
void	printServerAutoIndex(Config& data);
void	printServerMaxSize(Config& data);

void	printLocationRoot(ServerBlock& s);