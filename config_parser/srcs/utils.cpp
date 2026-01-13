/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 13:32:01 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/13 13:35:22 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void	printServerPorts(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (data.servers[i].port) {
			std::cout << "[DEBUG] server: " << (i + 1)
				<< " -> port: " << data.servers[i].port << std::endl;
		}
		else
			std::cout << "[DEBUG] server: " << (i + 1) << " -> No port!" << std::endl;

	}
}

void	printServerRoot(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (!data.servers[i].root.empty()) {
			std::cout << "[DEBUG] server: " << (i + 1)
			<< " -> root: " << data.servers[i].root << std::endl;
		}
		else
			std::cout << "[DEBUG] server: " << (i + 1) << " -> No root!" << std::endl;

	}
}

void	printServerIndex(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG] server: " << (i + 1) << " -> index: ";
		for (size_t j=0; j < data.servers[i].index.size(); ++j)
			std::cout << data.servers[i].index[j] << " ";
		std::cout << std::endl;
	}
}

void	printServerErrorPages(Config& data) {

	for (size_t i=0; i < data.servers.size(); ++i) {
		ServerBlock&	s = data.servers[i];

		if (s.errorPages.empty())
		{
  	 		std::cout << "[DEBUG] server: " << (i + 1) << " -> no error_page\n";
    		continue;
		}

		std::cout << "[DEBUG] server: " << (i + 1) << " -> error_page: ";

		std::map<int, StringVec>::iterator it = s.errorPages.begin();
		std::cout << it->first << " > ";
		for (size_t j=0; j < it->second.size(); ++j)
			std::cout << it->second[j] << " ";
		std::cout << std::endl;
	}
}

void	printServerAutoIndex(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG] server: " << (i + 1) << " -> autoindex: ";
		if (data.servers[i].autoIndex)
			std::cout << "on";
		else
			std::cout << "off";
		std::cout << std::endl;
	}
}


void	printServerMaxSize(Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG] server: " << (i + 1) << " -> MaxSize: ";

		size_t	bytes = data.servers[i].clientMaxBodySize;
		if (bytes >= (1024UL * 1024UL))
			std::cout << (bytes / (1024UL * 1024UL)) << "Mb" << std::endl;
		else
			std::cout << (bytes / 1024UL) << "Kb" << std::endl;
	}
}