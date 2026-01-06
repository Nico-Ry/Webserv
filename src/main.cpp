#include "../include/network/Server.hpp"
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
    int port = 8080;

    // Parser le port en argument
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Error: Invalid port number" << std::endl;
            std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
            return 1;
        }
    }

    try {
        // Configurer le gestionnaire de signaux
        signal(SIGINT, signal_handler);
        signal(SIGPIPE, SIG_IGN);  // Ignorer SIGPIPE

        // Creer et lancer le serveur
        Server server(port);
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