#include "ConfigParser.hpp"
#include <iomanip>

class Config;

void	printAllOutput(const Config& data);

void	printServerPorts(const Config& data);
void	printServerRoot(const Config& data);
void	printServerIndex(const Config& data);
void	printServerErrorPages(const Config& data);
void	printServerAutoIndex(const Config& data);
void	printServerMaxSize(const Config& data);

void	printLocationRoot(const ServerBlock& s);
void	printLocationIndex(const ServerBlock& s);
void	printLocationErrorPages(const ServerBlock& s);
void	printLocationAutoIndex(const ServerBlock& s);
void	printLocationMaxSize(const ServerBlock& s);
void	printLocationMethods(const ServerBlock& data);
void	printLocationRedirect(const ServerBlock& data);