#ifndef SERVER_HPP
#define SERVER_HPP

#include "SocketManager.hpp"
#include "Connection.hpp"
#include "IOMultiplexer.hpp"
#include "../http/RequestParser.hpp"
#include "../http/ResponseBuilder.hpp"
#include "../http/HttpResponse.hpp"
#include "../router/Router.hpp"
#include "../configParser/Config.hpp"
#include "../cgi/CgiProcess.hpp"
#include <map>
#include <vector>
#include <stdexcept>
#include <string>

class Router;

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
	void checkClientTimeouts();  // Vérifie et ferme les connexions inactives

	// CGI non-bloquant
	void handleCgiWrite(int pipe_fd);   // Ecrire body au CGI (POLLOUT sur pipe_in)
	void handleCgiRead(int pipe_fd);    // Lire output du CGI (POLLIN sur pipe_out)
	void checkCgiTimeouts();            // Verifier timeouts CGI
	void finishCgi(CgiProcess* cgi);    // Terminer un CGI et envoyer reponse
	void cleanupCgi(CgiProcess* cgi);   // Nettoyer un CGI (fermer pipes, kill process)
	bool isCgiPipe(int fd) const;       // Verifier si fd est un pipe CGI

	// Helper: verifie si un fd est un server socket
	bool isServerSocket(int fd) const;

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
	std::map<int, const ServerBlock*> client_to_server; // client_fd → ServerBlock

	// Multi-port support
	std::vector<int> server_fds;                    // Tous les server sockets
	std::map<int, const ServerBlock*> fd_to_server; // fd → ServerBlock config
	const Config* config;                           // Référence à la config complète

	// CGI non-bloquant
	std::map<int, CgiProcess*> cgi_by_pipe_in;      // pipe_in fd → CgiProcess
	std::map<int, CgiProcess*> cgi_by_pipe_out;     // pipe_out fd → CgiProcess
	std::map<int, CgiProcess*> cgi_by_client;       // client_fd → CgiProcess (pour savoir si client a un CGI en cours)

	bool running;

	// Copie interdite
	Server(const Server&);
	Server& operator=(const Server&);
};

#endif
