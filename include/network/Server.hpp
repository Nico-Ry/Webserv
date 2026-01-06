#ifndef SERVER_HPP
#define SERVER_HPP

#include "SocketManager.hpp"
#include "Connection.hpp"
#include "IOMultiplexer.hpp"
#include <map>
#include <stdexcept>
#include <string>

class Server {
public:
    class ServerException : public std::runtime_error {
    public:
        explicit ServerException(const std::string& message);
    };

    // Constructeur: initialise le serveur sur un port donne
    explicit Server(int port, int backlog = 128);

    // Destructeur: nettoie toutes les ressources
    ~Server();

    // Lance la boucle evenementielle du serveur
    void run();

    // Arrete le serveur proprement
    void stop();

private:
    // Gestionnaires d'evenements
    void acceptNewClient();
    void handleClientRead(int fd);
    void handleClientWrite(int fd);
    void removeClient(int fd);

    // Processus de la donnee recue (pour l'instant: simple echo)
    void processRequest(Connection* conn);

    // Membres
    SocketManager socket_manager;
    IOMultiplexer multiplexer;
    std::map<int, Connection*> clients;
    int server_fd;
    int port;
    bool running;

    // Copie interdite
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif