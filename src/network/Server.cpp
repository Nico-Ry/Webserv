#include "../../include/network/Server.hpp"
#include <iostream>
#include <cstring>

Server::ServerException::ServerException(const std::string& message)
    : std::runtime_error(message) {
}

Server::Server(int port, int backlog)
    : socket_manager(), multiplexer(), clients(), server_fd(-1), port(port), running(false) {

    std::cout << "=== Initializing Server on port " << port << " ===" << std::endl;

    // Creer le socket serveur
    server_fd = socket_manager.create_server(port, backlog);

    std::cout << "✓ Server socket created: fd=" << server_fd << std::endl;

    // Ajouter le server_fd au multiplexer pour surveiller les nouvelles connexions
    multiplexer.add_fd(server_fd, POLLIN);

    std::cout << "✓ Server ready to accept connections" << std::endl;
}

Server::~Server() {
    std::cout << "=== Shutting down server ===" << std::endl;

    // Fermer toutes les connexions clients
    for (std::map<int, Connection*>::iterator it = clients.begin();
         it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

    // Fermer le socket serveur
    if (server_fd >= 0) {
        SocketManager::close_socket(server_fd);
    }

    std::cout << "✓ Server stopped" << std::endl;
}

void Server::run() {
    running = true;

    std::cout << std::endl;
    std::cout << "Multi-client server running on port " << port << std::endl;
    std::cout << "Test with: telnet localhost " << port << std::endl;
    std::cout << "(Ctrl+C to stop)" << std::endl;
    std::cout << std::endl;

    while (running) {
        // Attendre des evenements sur les fds surveilles
        std::vector<int> ready_fds = multiplexer.wait(-1);

        if (ready_fds.empty()) {
            continue;
        }

        std::cout << "poll() returned " << ready_fds.size() << " ready fd(s)" << std::endl;

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

        std::cout << "✓ New client connected: fd=" << client_fd
                 << " (total clients: " << clients.size() << ")" << std::endl;
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
        std::cout << "  [fd=" << fd << "] Received " << n
                 << " bytes: \"" << conn->recv_buffer << "\"" << std::endl;

        // Traiter la requete (pour l'instant: echo simple)
        processRequest(conn);

        // Activer POLLOUT pour envoyer la reponse
        multiplexer.modify_fd(fd, POLLIN | POLLOUT);
    }
    else if (n == 0) {
        // Client a ferme la connexion
        std::cout << "  [fd=" << fd << "] Client disconnected" << std::endl;
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
            std::cout << "  [fd=" << fd << "] Sent " << sent << " bytes" << std::endl;
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

        std::cout << "  (remaining clients: " << clients.size() << ")" << std::endl;
    }
}

void Server::processRequest(Connection* conn) {
    // Pour l'instant: simple echo
    // Vos coequipiers pourront remplacer cette fonction par le parsing HTTP
    conn->send_buffer = conn->recv_buffer;
    conn->recv_buffer.clear();
}
