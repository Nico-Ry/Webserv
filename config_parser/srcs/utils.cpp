/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameechan <ameechan@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 13:32:01 by ameechan          #+#    #+#             */
/*   Updated: 2026/01/14 13:43:36 by ameechan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "colours.hpp"

void	printAllOutput(const Config& data) {
	std::cout << BOLD_GOLD << "~~~~~ SERVER BLOCKS ~~~~~" << RES << std::endl;

	std::cout << CYAN << "LISTEN" << RES << std::endl;
	printServerPorts(data);
	std::cout << "-------------------------" << std::endl;

	std::cout << CYAN << "ROOT" << RES << std::endl;
	printServerRoot(data);
	std::cout << "-------------------------" << std::endl;

	std::cout << CYAN << "INDEX" << RES << std::endl;
	printServerIndex(data);
	std::cout << "-------------------------" << std::endl;

	std::cout << CYAN << "ERROR_PAGE" << RES << std::endl;
	printServerErrorPages(data);
	std::cout << "-------------------------" << std::endl;

	std::cout << CYAN << "AUTOINDEX" << RES << std::endl;
	printServerAutoIndex(data);
	std::cout << "-------------------------" << std::endl;

	std::cout << CYAN << "MAX_SIZE" << RES << std::endl;
	printServerMaxSize(data);
	std::cout << "-------------------------\n" << std::endl;

	std::cout << BOLD_GOLD << "~~~~~ LOCATION BLOCKS ~~~~~" << RES << std::endl;
	for (size_t i=0; i < data.servers.size(); ++i) {
		ServerBlock	current = data.servers[i];
		std::cout << GREEN << "~SERVER " << i+1 << "~" << RES << std::endl;

		std::cout << CYAN << "ROOT" << RES << std::endl;
		printLocationRoot(current);
		std::cout << std::endl;

		std::cout << CYAN << "INDEX" << RES << std::endl;
		printLocationIndex(current);
		std::cout << std::endl;

		std::cout << CYAN << "ERROR_PAGE" << RES << std::endl;
		printLocationErrorPages(current);
		std::cout << std::endl;

		std::cout << CYAN << "AUTOINDEX" << RES << std::endl;
		printLocationAutoIndex(current);
		std::cout << std::endl;

		std::cout << CYAN << "MAX_SIZE" << RES << std::endl;
		printLocationMaxSize(current);
		std::cout << std::endl;

		std::cout << CYAN << "METHODS" << RES << std::endl;
		printLocationMethods(current);
		std::cout << std::endl;

		std::cout << CYAN << "REDIRECT" << RES << std::endl;
		printLocationRedirect(current);
		std::cout << "\n-------------------------" << std::endl;
	}
}


void	printServerPorts(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (data.servers[i].port) {
			std::cout << "[DEBUG] server: " << (i + 1)
				<< " -> port: " << data.servers[i].port << std::endl;
		}
		else
			std::cout << "[DEBUG] server: " << (i + 1) << " -> No port!" << std::endl;

	}
}

void	printServerRoot(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (!data.servers[i].root.empty()) {
			std::cout << "[DEBUG] server: " << (i + 1)
			<< " -> root: " << data.servers[i].root << std::endl;
		}
		else
			std::cout << "[DEBUG] server: " << (i + 1) << " -> No root!" << std::endl;

	}
}

void	printServerIndex(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG] server: " << (i + 1) << " -> index: ";
		for (size_t j=0; j < data.servers[i].index.size(); ++j)
			std::cout << data.servers[i].index[j] << " ";
		std::cout << std::endl;
	}
}

void	printServerErrorPages(const Config& data) {

	for (size_t i=0; i < data.servers.size(); ++i) {
		const ServerBlock&	s = data.servers[i];

		if (s.errorPages.empty())
		{
  	 		std::cout << "[DEBUG] server: " << (i + 1) << " -> no error_page\n";
    		continue;
		}

		std::cout << "[DEBUG] server: " << (i + 1) << " -> error_page: ";

		std::map<int, StringVec>::const_iterator it = s.errorPages.begin();
		std::cout << it->first << " > ";
		for (size_t j=0; j < it->second.size(); ++j)
			std::cout << it->second[j] << " ";
		std::cout << std::endl;
	}
}

void	printServerAutoIndex(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG] server: " << (i + 1) << " -> autoindex: ";
		if (data.servers[i].autoIndex)
			std::cout << "on";
		else
			std::cout << "off";
		std::cout << std::endl;
	}
}


void	printServerMaxSize(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG] server: " << (i + 1) << " -> MaxSize: ";

		size_t	bytes = data.servers[i].clientMaxBodySize;
		if (bytes >= (1024UL * 1024UL))
			std::cout << (bytes / (1024UL * 1024UL)) << "Mb" << std::endl;
		else
			std::cout << (bytes / 1024UL) << "Kb" << std::endl;
	}
}



void	printLocationRoot(const ServerBlock& s) {
	for (size_t i=0; i < s.locations.size(); ++i) {
		const LocationBlock&	current = s.locations[i];
		if (!current.root.empty()) {
			std::cout << "[DEBUG] location " << current.uri
			<< " -> root: " << current.root << std::endl;
		}
		else {
			std::cout << "[DEBUG] location: " << current.uri
				<< " -> NO root!" << std::endl;
		}
	}
}

void	printLocationIndex(const ServerBlock& s) {
	for (size_t i=0; i < s.locations.size(); ++i) {
		const LocationBlock&	current = s.locations[i];
		if (current.index.empty()) {
			std::cout << "[DEBUG] location " << current.uri
			<< " -> NO index!" << std::endl;
		}
		else {
			std::cout << "[DEBUG] location " << current.uri << " -> ";
			for (size_t i=0; i < current.index.size(); ++i)
				std::cout << current.index[i] << " ";
			std::cout << std::endl;
		}

	}
}

void	printLocationErrorPages(const ServerBlock& data) {

	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];

		if (l.errorPages.empty())
		{
  	 		std::cout << "[DEBUG] location " << l.uri << " -> no error_page\n";
    		continue;
		}

		std::cout << "[DEBUG] location " << l.uri << " -> error_page: ";

		std::map<int, StringVec>::const_iterator it = l.errorPages.begin();
		std::cout << it->first << " > ";
		for (size_t j=0; j < it->second.size(); ++j)
			std::cout << it->second[j] << " ";
		std::cout << std::endl;
	}
}

void	printLocationAutoIndex(const ServerBlock& data) {
	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];
		std::cout << "[DEBUG] location " << l.uri << " -> autoindex: ";
		if (data.locations[i].autoIndex)
			std::cout << "on";
		else
			std::cout << "off";
		std::cout << std::endl;
	}
}

void	printLocationMaxSize(const ServerBlock& data) {
	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock& l = data.locations[i];
		std::cout << "[DEBUG] location " << l.uri << " -> MaxSize: ";

		size_t	bytes = data.locations[i].clientMaxBodySize;
		if (bytes >= (1024UL * 1024UL))
			std::cout << (bytes / (1024UL * 1024UL)) << "Mb" << std::endl;
		else
			std::cout << (bytes / 1024UL) << "Kb" << std::endl;
	}
}


void	printLocationMethods(const ServerBlock& data) {

	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];

		if (l.methods.empty())
		{
  	 		std::cout << "[DEBUG] location " << l.uri << " -> methods: DEFAULT (GET)!\n";
    		continue;
		}

		std::cout << "[DEBUG] location " << l.uri << " -> methods: ";
		for (size_t i=0; i < l.methods.size(); ++i)
			std::cout << l.methods[i] << " ";
		std::cout << std::endl;
	}
}

void	printLocationRedirect(const ServerBlock& data) {

	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];

		if (!l.hasRedirect)
		{
  	 		std::cout << "[DEBUG] location " << l.uri << " -> redirect: NO redirect\n";
    		continue;
		}

		std::cout << "[DEBUG] location " << l.uri << " -> redirect: "
			<< l.redirectCode << " " << l.redirectTarget << std::endl;
	}
}