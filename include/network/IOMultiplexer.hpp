#ifndef IOMULTIPLEXER_HPP
#define IOMULTIPLEXER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include <stdexcept>
#include <string>

class IOMultiplexer {
public:
    class MultiplexerException : public std::runtime_error {
    public:
        explicit MultiplexerException(const std::string& message);
    };

    IOMultiplexer();
    ~IOMultiplexer();

    // Ajoute un fd a surveiller avec les events specifies (POLLIN, POLLOUT, etc.)
    void add_fd(int fd, short events);

    // Retire un fd de la surveillance
    void remove_fd(int fd);

    // Modifie les events surveilles pour un fd
    void modify_fd(int fd, short events);

    // Attend des evenements sur les fds surveilles
    // timeout: -1 = infini, 0 = retour immediat, >0 = timeout en ms
    // Retourne les fds qui ont des evenements
    std::vector<int> wait(int timeout);

    // Recupere les revents (evenements reels) pour un fd
    short get_revents(int fd) const;

    // Verifie si un fd est surveille
    bool has_fd(int fd) const;

    // Nombre de fds surveilles
    size_t size() const;

private:
    std::vector<struct pollfd> fds;
    std::map<int, size_t> fd_to_index;

    // Copie interdite
    IOMultiplexer(const IOMultiplexer&);
    IOMultiplexer& operator=(const IOMultiplexer&);
};

#endif