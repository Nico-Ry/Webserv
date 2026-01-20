#ifndef SERVER_HPP
#define SERVER_HPP

#include "SocketManager.hpp"
#include "Connection.hpp"
#include "IOMultiplexer.hpp"
#include "../http/RequestParser.hpp"
#include "../http/ResponseBuilder.hpp"
#include "../http/Response.hpp"
#include "../configParser/Config.hpp"
#include <map>
#include <vector>
#include <stdexcept>
#include <string>


class Server {
public:
    class ServerException : public std::runtime_error {
    public:
        explicit ServerException(const std::string& message);
    };

    // Constructeur: initialise le serveur avec une configuration (multi-ports)
    explicit Server(const Config& cfg, int backlog = 128);

    // Destructeur: nettoie toutes les ressources
    ~Server();

    // Lance la boucle evenementielle du serveur
    void run();

    // Arrete le serveur proprement
    void stop();

private:
    // Gestionnaires d'evenements
    void acceptNewClient(int server_fd);
    void handleClientRead(int fd);
    void handleClientWrite(int fd);
    void removeClient(int fd);

    // Helper: verifie si un fd est un server socket
    bool isServerSocket(int fd) const;

    // Processus de la donnee recue - utilise HttpRequestParser
    void processRequest(Connection* conn, int fd);

    // Traite une requete HTTP complete et retourne une reponse
    // TODO: Plus tard, cette fonction appellera le Router
    HttpResponse handleHttpRequest(const HttpRequest& req);

    // Membres
    SocketManager socket_manager;
    IOMultiplexer multiplexer;
    std::map<int, Connection*> clients;
    std::map<int, HttpRequestParser*> parsers;  // Un parser par client (keep-alive)

    // Multi-port support
    std::vector<int> server_fds;                    // Tous les server sockets
    std::map<int, const ServerBlock*> fd_to_server; // fd → ServerBlock config
    const Config* config;                           // Référence à la config complète

    bool running;

    // Copie interdite
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif