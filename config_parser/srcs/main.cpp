/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:27:33 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/09 18:16:38 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

int	main(int ac, char** av) {
	if (ac != 2)
		return -1;
	try {
		Config	cfg(av[1]);
		std::cout << "[DEBUG] PARSING SUCCESS" << std::endl;
	}
	catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
	return 0;
}