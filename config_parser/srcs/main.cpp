/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:33 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/07 17:33:30 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

int	main(int ac, char** av) {
	if (ac != 2)
		return -1;

	Config	base;
	base.validInputFile(av[1]);
	base.parseConfigFile();

	return 0;
}