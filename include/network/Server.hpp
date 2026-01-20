#ifndef SERVER_HPP
#define SERVER_HPP

#include "SocketManager.hpp"
#include "Connection.hpp"
#include "IOMultiplexer.hpp"
#include "../http/RequestParser.hpp"
#include "../http/ResponseBuilder.hpp"
#include "../http/Response.hpp"
#include "../router/Router.hpp"
#include "../configParser/Config.hpp"
#include <map>
#include <stdexcept>
#include <string>

class Router;

class Server {
public:
    class ServerException : public std::runtime_error {
    public:
        explicit ServerException(const std::string& message);
    };

	const Config&	cfg;

    // Constructeur: initialise le serveur sur un port donne
    explicit Server(int port, const Config& cfg, int backlog = 128);

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

    // Processus de la donnee recue - utilise HttpRequestParser
    void processRequest(Connection* conn, int fd);

    // Traite une requete HTTP complete et retourne une reponse
    // TODO: Plus tard, cette fonction appellera le Router
    // HttpResponse handleHttpRequest(const HttpRequest& req);

    // Membres
    SocketManager socket_manager;
    IOMultiplexer multiplexer;
    std::map<int, Connection*> clients;
    std::map<int, HttpRequestParser*> parsers;  // Un parser par client (keep-alive)
    int server_fd;
    int port;
    bool running;

    // Copie interdite
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif