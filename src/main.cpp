/*
High-level configuration parsing flow:

1) A Config object is created. (Config.cpp/hpp)
   - Config is the top-level container and stores all ServerBlock objects.

2) The configuration file is tokenised. (Tokeniser.cpp/hpp)
   - Tokeniser reads the raw config file.
   - It produces a vector of Token objects (keywords, values, symbols).
   - No semantic meaning is applied at this stage.

3) The token stream is parsed. (ConfigParser.cpp/hpp)
   - ConfigParser consumes tokens sequentially.
   - Server blocks are parsed first. (parseServerBlock.cpp)
   - Each ServerBlock parses its own directives and nested LocationBlocks. (parseLocationBlock.cpp)
   - Each directive is handled by a dedicated parsing function.
   - Syntax and semantic errors are detected here.

Data structure layout:

Config															 (Config.hpp)
└── vector<ServerBlock>												  |
    └── ServerBlock											(ServerBlock.hpp)
        ├── server-level configuration/info							  |
        └── vector<LocationBlock>									  |
            └── LocationBlock							  (LocationBlock.hpp)
                └── location-level configuration/info

Notes:
- LocationBlocks represent more specific configuration than ServerBlocks.
- Inheritance and fallback (server -> location) are resolved at runtime,
  not during parsing.
- Parsing only builds and validates the configuration structure.
*/



#include "../include/network/Server.hpp"
#include "configParser/Config.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>

// Variable globale pour gerer l'arret propre
static Server* g_server = NULL;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << std::endl << "Received SIGINT, stopping server..." << std::endl;
        if (g_server) {
            g_server->stop();
        }
    }
}



int main(int argc, char** argv) {
    // int port = 8080;

    // // Parser le port en argument
    // if (argc > 1) {
    //     port = std::atoi(argv[1]);
    //     if (port <= 0 || port > 65535) {
    //         std::cerr << "Error: Invalid port number" << std::endl;
    //         std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
    //         return 1;
    //     }
    // }

	if (argc != 2)
		return -1;

    try {

		// Read config file passed as argument and build Config
		std::string	configFile(argv[1]);
		Config	cfg(configFile);
		// printAllOutput(cfg);//		 <-- Uncomment to print all parsed output



        // Configurer le gestionnaire de signaux
        signal(SIGINT, signal_handler);
        signal(SIGPIPE, SIG_IGN);  // Ignorer SIGPIPE

        // Creer et lancer le serveur multi-ports
        Server server(cfg); // Passe toute la config (supporte multi-ports)
        g_server = &server;

        server.run();

        g_server = NULL;

    } catch (const Server::ServerException& e) {
        std::cerr << "✗ Server Error: " << e.what() << std::endl;
        return 1;
    } catch (const SocketManager::SocketException& e) {
        std::cerr << "✗ Socket Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "✗ Unexpected Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}