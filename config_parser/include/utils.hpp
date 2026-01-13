/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 13:30:58 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 18:01:38 by ameechan         ###   ########.fr       */
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
void	printLocationIndex(ServerBlock& s);
void	printLocationErrorPages(ServerBlock& s);
void	printLocationAutoIndex(ServerBlock& s);
void	printLocationMaxSize(ServerBlock& s);
void	printLocationMethods(ServerBlock& data);
void	printLocationRedirect(ServerBlock& data);