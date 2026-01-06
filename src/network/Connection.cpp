#include "../../include/network/Connection.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>

Connection::ConnectionException::ConnectionException(const std::string& message)
    : std::runtime_error(message) {
}

Connection::Connection(int fd)
    : fd(fd), recv_buffer(), send_buffer(), bytes_sent(0) {
    set_nonblocking();
}

Connection::~Connection() {
    if (fd >= 0) {
        close(fd);
    }
}

void Connection::set_nonblocking() {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        throw ConnectionException("fcntl(F_GETFL) failed: " + std::string(strerror(errno)));
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        throw ConnectionException("fcntl(F_SETFL, O_NONBLOCK) failed: " + std::string(strerror(errno)));
    }
}

ssize_t Connection::read_available() {
    char buffer[4096];
    ssize_t total_read = 0;

    while (true) {
        ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

        if (n > 0) {
            recv_buffer.append(buffer, n);
            total_read += n;
        }
        else if (n == 0) {
            // Connexion fermee par le client
            return 0;
        }
        else {
            // n < 0: erreur
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Pas d'erreur: juste plus de donnees disponibles
                break;
            }
            // Vraie erreur
            return -1;
        }
    }

    return total_read;
}

ssize_t Connection::write_pending() {
    if (send_buffer.empty() || bytes_sent >= send_buffer.length()) {
        return 0;
    }

    ssize_t total_sent = 0;

    while (bytes_sent < send_buffer.length()) {
        ssize_t n = send(fd,
                        send_buffer.data() + bytes_sent,
                        send_buffer.length() - bytes_sent,
                        MSG_NOSIGNAL);

        if (n > 0) {
            bytes_sent += n;
            total_sent += n;
        }
        else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Socket buffer plein, on attendra le prochain POLLOUT
                break;
            }
            // Vraie erreur
            return -1;
        }
    }

    // Si tout a ete envoye, nettoyer les buffers
    if (bytes_sent >= send_buffer.length()) {
        send_buffer.clear();
        bytes_sent = 0;
    }

    return total_sent;
}

bool Connection::has_pending_data() const {
    return !send_buffer.empty() && bytes_sent < send_buffer.length();
}