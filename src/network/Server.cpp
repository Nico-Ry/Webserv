#include "network/Server.hpp"
#include "http/Status.hpp"
#include "colours.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <sstream>

// Helper function pour convertir int en string (C++98)
static std::string intToString(int n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

Server::ServerException::ServerException(const std::string& message)
    : std::runtime_error(message) {
}

Server::Server(int port, const Config& cfg, int backlog)
    : cfg(cfg), socket_manager(), multiplexer(), clients(), server_fd(-1), port(port), running(false) {

    std::cout << BOLD_CYAN << "=== Initializing Server on port ["
		<< port << "] ===" << RES << std::endl;

    // Creer le socket serveur
    server_fd = socket_manager.create_server(port, backlog);

    std::cout << GREEN << "✓ " << RES << "Server socket created: "
		<< RES << "fd=" << server_fd << std::endl;

    // Ajouter le server_fd au multiplexer pour surveiller les nouvelles connexions
    multiplexer.add_fd(server_fd, POLLIN);

    std::cout << GREEN << "✓ " << RES
		<< "Server ready to accept connections" << std::endl;
}

Server::~Server() {
    std::cout << BOLD_ORANGE << "=== Shutting down server ===" << RES << std::endl;

    // Fermer toutes les connexions clients
    for (std::map<int, Connection*>::iterator it = clients.begin();
         it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

    // Liberer tous les parsers HTTP
    for (std::map<int, HttpRequestParser*>::iterator it = parsers.begin();
         it != parsers.end(); ++it) {
        delete it->second;
    }
    parsers.clear();

    // Fermer le socket serveur
    if (server_fd >= 0) {
        SocketManager::close_socket(server_fd);
    }

    std::cout << GREEN << "✓ " << RES << "Server stopped" << std::endl;
}

void Server::run() {
    running = true;

    std::cout << std::endl
    	<< "Multi-client server running on port "
		<< GREEN << port << RES << std::endl
    	<< "Test with: telnet localhost "
		<< GREEN << port << RES << std::endl
    	<< "(Ctrl+C to stop)\n" << std::endl;

    while (running) {
        // Attendre des evenements sur les fds surveilles
        std::vector<int> ready_fds = multiplexer.wait(-1);

        if (ready_fds.empty()) {
            continue;
        }

        // std::cout << "poll() returned " << ready_fds.size() << " ready fd(s)" << std::endl;

        // Traiter chaque fd pret
        for (size_t i = 0; i < ready_fds.size(); i++) {
            int fd = ready_fds[i];

            if (fd == server_fd) {
                // Nouvelle connexion entrante
                acceptNewClient();
            }
            else {
                // Evenement sur un client existant
                short revents = multiplexer.get_revents(fd);
                bool client_disconnected = false;

                if (revents & POLLIN) {
                    handleClientRead(fd);

                    // Verifier si le client a ete supprime pendant la lecture
                    if (clients.find(fd) == clients.end()) {
                        client_disconnected = true;
                    }
                }

                // IMPORTANT: ne traiter POLLOUT que si le client n'a pas ete deconnecte
                if (!client_disconnected && (revents & POLLOUT)) {
                    handleClientWrite(fd);
                }
            }
        }

        std::cout << std::endl;
    }
}

void Server::stop() {
    running = false;
}

void Server::acceptNewClient() {
    try {
        int client_fd = socket_manager.accept_connection(server_fd);
        Connection* conn = new Connection(client_fd);
        clients[client_fd] = conn;

        // Surveiller le client pour les donnees entrantes
        multiplexer.add_fd(client_fd, POLLIN);

        std::cout << GREEN << "✓ " << RES << "New client connected: "
			<< "fd=" << client_fd << " -> (total clients: " << clients.size() << ")"
			<< std::endl;
    }
    catch (const SocketManager::SocketException& e) {
        std::cerr << "✗ Error accepting client: " << e.what() << std::endl;
    }
    catch (const Connection::ConnectionException& e) {
        std::cerr << "✗ Error creating connection: " << e.what() << std::endl;
    }
}

void Server::handleClientRead(int fd) {
    Connection* conn = clients[fd];
    ssize_t n = conn->read_available();

    if (n > 0) {
        // std::cout << "  [fd=" << fd << "] Received " << n << " bytes" << std::endl;
        std::cout << BOLD_YELLOW << "[DEBUG] "
			<< RES << "Received " << n << " bytes" << std::endl;

        // Traiter la requete HTTP avec le parser
        processRequest(conn, fd);

        // Activer POLLOUT si une reponse est prete
        if (!conn->send_buffer.empty()) {
            multiplexer.modify_fd(fd, POLLIN | POLLOUT);
        }
    }
    else if (n == 0) {
        // Client a ferme la connexion
        // std::cout << "  [fd=" << fd << "] Client disconnected" << std::endl;
        std::cout << BOLD_YELLOW << "[DEBUG] " << RES << "Client disconnected" << std::endl;
        removeClient(fd);
    }
    else {
        // Erreur de lecture
        std::cout << "  [fd=" << fd << "] Error reading" << std::endl;
        removeClient(fd);
    }
}

void Server::handleClientWrite(int fd) {
    Connection* conn = clients[fd];

    if (conn->has_pending_data()) {
        ssize_t sent = conn->write_pending();

        if (sent > 0) {
            // std::cout << "  [fd=" << fd << "] Sent " << sent << " bytes" << std::endl;
            std::cout << BOLD_YELLOW << "[DEBUG] "
				<< RES << "Sent " << sent << " bytes" << std::endl;
        }
        else if (sent < 0) {
            std::cout << "  [fd=" << fd << "] Error writing" << std::endl;
            removeClient(fd);
            return;
        }

        // Si tout a ete envoye, desactiver POLLOUT
        if (!conn->has_pending_data()) {
            multiplexer.modify_fd(fd, POLLIN);
        }
    }
    else {
        // Plus rien a envoyer, desactiver POLLOUT
        multiplexer.modify_fd(fd, POLLIN);
    }
}

void Server::removeClient(int fd) {
    std::map<int, Connection*>::iterator it = clients.find(fd);

    if (it != clients.end()) {
        multiplexer.remove_fd(fd);
        delete it->second;
        clients.erase(it);

        // Supprimer aussi le parser HTTP associe
        std::map<int, HttpRequestParser*>::iterator parser_it = parsers.find(fd);
        if (parser_it != parsers.end()) {
            delete parser_it->second;
            parsers.erase(parser_it);
        }

        std::cout << BOLD_YELLOW << "[DEBUG] "
			<< RES << "live clients: " << clients.size() << std::endl;
        // std::cout << "  (remaining clients: " << clients.size() << ")" << std::endl;
    }
}

void Server::processRequest(Connection* conn, int fd) {
    // Creer un parser pour ce client si necessaire
    if (parsers.find(fd) == parsers.end()) {
        parsers[fd] = new HttpRequestParser();
        // std::cout << "  [fd=" << fd << "] Created HTTP parser" << std::endl;
    }

    HttpRequestParser* parser = parsers[fd];

    // Envoyer les donnees au parser
	// std::cout << BOLD_GOLD << conn->recv_buffer << RES << std::endl;
    parser->feed(conn->recv_buffer);
    conn->recv_buffer.clear();


	if (parser->hasError()) {
		// Generer une reponse d'erreur HTTP
		int errorCode = parser->getErrorStatus();
		HttpResponse resp(errorCode, reasonPhrase(errorCode));


	// Build HTML Error Page
		resp.headers["Content-Type"] = "text/html";
		resp.body = "<html><body><h1>Error " + intToString(errorCode)
					+ "</h1><p>" + reasonPhrase(errorCode) + "</p></body></html>";

		// std::cout << "  [fd=" << fd << "] HTTP Error: " << errorCode << std::endl;
		std::cerr << RED << resp.statusCode << RES << " " << resp.reason << std::endl;
		// Toujours fermer connexion sur erreur
		conn->send_buffer = ResponseBuilder::build(resp, true);
	}

    // Verifier si la requete est complete
    else if (parser->isDone()) {

		// Traiter la requete HTTP valide
		const HttpRequest& req = parser->getRequest();

		// std::cout << "  [fd=" << fd << "] HTTP Request: "
		//          << (req.method == METHOD_GET ? "GET" :
		//              req.method == METHOD_POST ? "POST" :
		//              req.method == METHOD_DELETE ? "DELETE" : "UNKNOWN")
		//          << " " << req.rawTarget << " " << req.httpVersion << std::endl;
		printHttpRequest(req);

		// Generer la reponse HTTP
		Router	requestHandler(cfg, this->port);
		HttpResponse resp = requestHandler.buildResponse(req);

		// Determiner si on doit fermer la connexion (keep-alive)
		bool closeConnection = parser->shouldCloseConnection();
		conn->send_buffer = ResponseBuilder::build(resp, closeConnection);//TODO: NEED TO ADAPT BEHAVIOUR FOR DIFFERNT STATUS CODES

		// std::cout << "  [fd=" << fd << "] HTTP Response: " << resp.statusCode
		//          << " (Connection: " << (closeConnection ? "close" : "keep-alive") << ")" << std::endl;
		std::cout << BOLD_GREEN << "[HTTP Response] " << RES << resp.statusCode
					<< " (Connection: " << (closeConnection ? "close" : "keep-alive") << ")" << std::endl;


        // Reset parser pour la prochaine requete (keep-alive)
        parser->reset();
    }
}

// ============================================================================
// ROUTING TEMPORAIRE - handleHttpRequest()
// ============================================================================
// CETTE FONCTION EST TEMPORAIRE !
//
// Actuellement: On fait un routing basique en C++ avec des if/else sur le path
// - "/" → Page d'accueil avec menu (cards cliquables)
// - "/test" → Page de test avec boutons GET/POST/DELETE
// - "/upload" → Page d'upload (simulation, pas encore fonctionnel)
// - Autre → Page 404
//
// FUTUR (à implémenter par ton équipe):
// Cette fonction devra être remplacée par :
//
//   HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
//       return router->route(req);  // ← Simple appel au Router
//   }
//
// Et le Router fera:
// 1. Matcher le path avec la config (location blocks)
// 2. Vérifier que la méthode est autorisée
// 3. Dispatcher vers le bon handler:
//    - FileHandler pour GET (servir fichiers HTML/CSS/JS depuis disque)
//    - UploadHandler pour POST /upload (sauvegarder fichiers)
//    - CGIHandler pour *.py, *.php (exécuter scripts)
//    - ErrorHandler pour 404, 403, 500, etc.
//
// Exemple d'architecture finale :
//
//   Router::route(req) {
//       LocationConfig loc = config->matchLocation(req.path);
//
//       if (!loc.isMethodAllowed(req.method))
//           return errorHandler->methodNotAllowed(405);
//
//       if (req.method == GET && isCGI(req.path))
//           return cgiHandler->execute(req, loc);
//       else if (req.method == GET)
//           return fileHandler->serveFile(req.path, loc);
//       else if (req.method == POST && req.path == "/upload")
//           return uploadHandler->handle(req, loc);
//       else if (req.method == DELETE)
//           return fileHandler->deleteFile(req.path, loc);
//
//       return errorHandler->notFound(404);
//   }
//
// ============================================================================
// HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
//     HttpResponse resp(200, "OK");
//     resp.headers["Content-Type"] = "text/html; charset=utf-8";

//     // Headers CORS pour permettre les tests depuis le navigateur
//     resp.headers["Access-Control-Allow-Origin"] = "*";
//     resp.headers["Access-Control-Allow-Methods"] = "GET, POST, DELETE, OPTIONS";
//     resp.headers["Access-Control-Allow-Headers"] = "Content-Type";

//     // ========================================================================
//     // ROUTING TEMPORAIRE (if/else basique sur le path)
//     // ========================================================================
//     // Route: "/" → Page d'accueil (menu principal)
//     if (req.rawTarget == "/") {
//         resp.body = generateHomePage();
//     }
//     // Route: "/test" → Page de test avec boutons interactifs
//     else if (req.rawTarget == "/test") {
//         resp.body = generateTestPage(req);
//     }
//     // Route: "/upload" → Page d'upload (simulation)
//     else if (req.rawTarget == "/upload") {
//         resp.body = generateUploadPage(req);
//     }
//     // Route: favicon (éviter 404 pour favicon)
//     else if (req.rawTarget == "/favicon.ico") {
//         resp.statusCode = 204;  // No Content
//         resp.reason = "No Content";
//         resp.body = "";
//     }
//     // Route: Tout le reste → 404 Not Found
//     else {
//         resp.statusCode = 404;
//         resp.reason = "Not Found";
//         resp.body = generate404Page(req.rawTarget);
//     }

//     return resp;
// }
