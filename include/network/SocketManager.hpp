#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class SocketManager {
public:
    class SocketException : public std::runtime_error {
    public:
        explicit SocketException(const std::string& message);
    };

    SocketManager();
    ~SocketManager();

    // Crée un socket serveur complet (create + configure + bind + listen)
    int create_server(int port, int backlog);

    // Accepte une nouvelle connexion
    int accept_connection(int server_fd);

    // Ferme un socket
    static void close_socket(int fd);

private:
    // Crée un nouveau socket TCP
    int create_socket();

    // Configure le socket avec SO_REUSEADDR
    void configure_socket(int fd);

    // Bind le socket à un port
    void bind_socket(int fd, int port);

    // Met le socket en mode écoute
    void start_listening(int fd, int backlog);
};

#endif