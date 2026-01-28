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

	std::cout << CYAN << "UPLOAD DIRECTORY" << RES << std::endl;
	printServerUpload(data);
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

		std::cout << CYAN << "UPLOAD DIRECTORY" << RES << std::endl;
		printLocationUpload(current);
		std::cout << std::endl;

		std::cout << CYAN << "REDIRECT" << RES << std::endl;
		printLocationRedirect(current);
		std::cout << "\n-------------------------" << std::endl;
	}
	std::cout << BOLD_GREEN << "PARSING SUCCESS"
		<< RES << "\n-------------------------" << std::endl;
}


void	printServerPorts(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (data.servers[i].port) {
			std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES
				<< "-> " << GREEN << data.servers[i].port << RES << std::endl;
		}
		else{
			std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: "
				<< RES << "-> " << RED << "No port!" << RES << std::endl;
		}

	}
}

void	printServerRoot(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (!data.servers[i].root.empty()) {
			std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES
			<< "-> " << GREEN << data.servers[i].root << RES << std::endl;
		}
		else{
			std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: "
				<< RES << "-> " << RED << "No root!" << RES << std::endl;
		}

	}
}

void	printServerUpload(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		if (!data.servers[i].uploadDir.empty()) {
			std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES
			<< "-> " << GREEN << data.servers[i].uploadDir << RES << std::endl;
		}
		else{
			std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: "
				<< RES << "-> " << RED << "No Upload Directory!" << RES << std::endl;
		}

	}
}

void	printServerIndex(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		const ServerBlock&	current = data.servers[i];
		std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES << "-> ";

		if (current.index.empty())
			std::cout << RED << "No Index Files!" << RES << std::endl;
		else {
			for (size_t j=0; j < current.index.size(); ++j)
				std::cout << GREEN << current.index[j] << " ";
		}
		std::cout << RES << std::endl;
	}
}

void	printServerErrorPages(const Config& data) {

	for (size_t i=0; i < data.servers.size(); ++i) {
		const ServerBlock&	s = data.servers[i];

		if (s.errorPages.empty())
		{
  	 		std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: "
				<< RES << "-> " << RED << "no error_page\n" << RES;
    		continue;
		}

		std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES << "-> ";

		std::map<int, StringVec>::const_iterator it = s.errorPages.begin();
		while (it != s.errorPages.end()) {

			if (it == s.errorPages.begin())
				std::cout << PURPLE << it->first << RES << " > ";
			else
				std::cout << std::setw(33) << PURPLE << it->first << RES << " > ";
			for (size_t j=0; j < it->second.size(); ++j) {
				std::cout << GREEN << it->second[j] << " ";
			}
			std::cout << RES << std::endl;
			++it;
		}
	}
}

void	printServerAutoIndex(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES << "-> ";
		if (data.servers[i].autoIndex)
			std::cout << GREEN << "on" << RES;
		else
			std::cout << RED << "off" << RES;
		std::cout << std::endl;
	}
}


void	printServerMaxSize(const Config& data) {
	for (size_t i=0; i < data.servers.size(); ++i) {
		std::cout << "[DEBUG]" << YELLOW << " server[" << (i + 1) << "]: " << RES << "-> ";

		size_t	bytes = data.servers[i].clientMaxBodySize;
		if (bytes >= (1024UL * 1024UL))
			std::cout << GREEN << (bytes / (1024UL * 1024UL)) << "Mb" << RES << std::endl;
		else
			std::cout << MAGENTA << (bytes / 1024UL) << "Kb" << RES << std::endl;
	}
}



void	printLocationRoot(const ServerBlock& s) {
	for (size_t i=0; i < s.locations.size(); ++i) {
		const LocationBlock&	current = s.locations[i];
		if (!current.root.empty()) {
			std::cout << BOLD << "[Location] " << RES << YELLOW << current.uri << RES
			<< " -> " << GREEN << current.root << RES << std::endl;
		}
		else {
			std::cout << BOLD << "[Location] "<< RES << YELLOW << current.uri
				<< RES << " -> " << RED << "NO root!" << RES << std::endl;
		}
	}
}

void	printLocationUpload(const ServerBlock& s) {
	for (size_t i=0; i < s.locations.size(); ++i) {
		const LocationBlock&	current = s.locations[i];
		if (!current.uploadDir.empty()) {
			std::cout << BOLD << "[Location] " << RES << YELLOW << current.uri << RES
			<< " -> " << GREEN << current.uploadDir << RES << std::endl;
		}
		else {
			std::cout << BOLD << "[Location] "<< RES << YELLOW << current.uri
				<< RES << " -> " << RED << "NO Upload Directory!" << RES << std::endl;
		}
	}
}

void	printLocationIndex(const ServerBlock& s) {
	for (size_t i=0; i < s.locations.size(); ++i) {
		const LocationBlock&	current = s.locations[i];
		if (current.index.empty()) {
			std::cout << BOLD << "[Location] "<< RES << YELLOW << current.uri << RES
			<< " -> " << RED << "NO index!" << RES << std::endl;
		}
		else {
			std::cout << BOLD << "[Location] "<< RES << YELLOW << current.uri << RES << " -> ";
			for (size_t i=0; i < current.index.size(); ++i)
				std::cout << GREEN << current.index[i] << " ";
			std::cout << RES << std::endl;
		}

	}
}

void	printLocationErrorPages(const ServerBlock& data) {

	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];

		if (l.errorPages.empty())
		{
  	 		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri
				<< RES << " -> " << RED << "no error_page\n" << RES;
    		continue;
		}

		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri << RES << " -> ";

		std::map<int, StringVec>::const_iterator it = l.errorPages.begin();
		while (it != l.errorPages.end()) {

			if (it == l.errorPages.begin())
				std::cout << PURPLE << it->first << RES << " > ";
			else
				std::cout << std::setw(33) << PURPLE << it->first << RES << " > ";
			for (size_t j=0; j < it->second.size(); ++j) {
				std::cout << GREEN << it->second[j] << " ";
			}
			std::cout << RES << std::endl;
			++it;
		}
		std::cout << std::endl;
	}
}

void	printLocationAutoIndex(const ServerBlock& data) {
	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];
		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri << RES << " -> ";
		if (data.locations[i].autoIndex)
			std::cout << GREEN << "on" << RES;
		else
			std::cout << RED << "off" << RES;
		std::cout << std::endl;
	}
}

void	printLocationMaxSize(const ServerBlock& data) {
	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock& l = data.locations[i];
		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri << RES << " -> ";

		size_t	bytes = data.locations[i].clientMaxBodySize;
		if (bytes >= (1024UL * 1024UL))
			std::cout << GREEN << (bytes / (1024UL * 1024UL)) << "Mb" << RES << std::endl;
		else
			std::cout << MAGENTA << (bytes / 1024UL) << "Kb" << RES << std::endl;
	}
}


void	printLocationMethods(const ServerBlock& data) {

	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];

		if (l.methods.empty())
		{
  	 		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri
				<< RES << " -> " << ORANGE << "DEFAULT (GET)!\n" << RES;
    		continue;
		}

		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri << RES << " -> ";
		for (size_t i=0; i < l.methods.size(); ++i)
			std::cout << GREEN << l.methods[i] << " ";
		std::cout << RES << std::endl;
	}
}

void	printLocationRedirect(const ServerBlock& data) {

	for (size_t i=0; i < data.locations.size(); ++i) {
		const LocationBlock&	l = data.locations[i];

		if (!l.hasRedirect)
		{
  	 		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri
				<< RES << " -> " << RED << "NO redirect\n" << RES;
    		continue;
		}

		std::cout << BOLD << "[Location] " << RES << YELLOW << l.uri << RES << " -> "
			<< PURPLE << l.redirectCode << " "
			<< GREEN << l.redirectTarget << RES << std::endl;
	}
}