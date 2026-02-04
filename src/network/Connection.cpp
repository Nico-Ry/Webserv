#include "../../include/network/Connection.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>

Connection::ConnectionException::ConnectionException(const std::string& message)
	: std::runtime_error(message)
{}
// Constructeur: met automatiquement le fd en mode non-bloquant
Connection::Connection(int fd)
	:	fd(fd),
		totalBytesReceived(0),
		recv_buffer(),
		send_buffer(),
		bytes_sent(0),
		last_activity(time(NULL)),
		should_close(false)
{
	set_nonblocking();
}

// Met à jour le timestamp d'activité
void Connection::update_activity()
{
	last_activity = time(NULL);
}

// Destructeur: ferme le fd
Connection::~Connection()
{
	if (fd >= 0)
	{
		close(fd);
	}
}

// Met le socket en mode non-bloquant
void Connection::set_nonblocking()
{
	// Note: subject only allows F_SETFL, O_NONBLOCK, FD_CLOEXEC (no F_GETFL)
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		throw ConnectionException("fcntl(F_SETFL, O_NONBLOCK) failed: " + std::string(strerror(errno)));
	}
}
#include "utils.hpp"
#include "http/ResponseBuilder.hpp"
/**
 * @brief Lit toutes les donnees disponibles dans recv_buffer
 * @return >0 bytes lus, 0 si connexion fermee, -1 si erreur
 */
ssize_t Connection::read_available()
{
	size_t	bufferSize = 4096;
	char	buffer[bufferSize];

	// Un seul appel recv() par evenement POLLIN
	// On ne verifie JAMAIS errno apres recv() (interdit par le sujet)
	ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

	if (n == 0)
		return 0; // connexion fermee par le client
	if (n < 0)
		return -1; // erreur - fermer la connexion

	// std::cout << BOLD_GOLD << totalBytesReceived << RES << std::endl;

	recv_buffer.append(buffer, n);
	totalBytesReceived += recv_buffer.size();
	if (totalBytesReceived > maxRequestSize) {
		return -2; // error payload too large
	}

	return n;
}


/**
 * @brief Envoie les donnees de send_buffer
 * @return >0 bytes envoyes, 0 si rien a envoyer, -1 si erreur
 */
ssize_t Connection::write_pending()
{
	if (send_buffer.empty() || bytes_sent >= send_buffer.length())
	{
		return 0;
	}

	// Un seul appel send() par evenement POLLOUT
	// On ne verifie JAMAIS errno apres send() (interdit par le sujet)
	ssize_t n = send(fd,
					send_buffer.data() + bytes_sent,
					send_buffer.length() - bytes_sent,
					MSG_NOSIGNAL);

	if (n > 0)
	{
		bytes_sent += n;

		// Si tout a ete envoye, nettoyer les buffers
		if (bytes_sent >= send_buffer.length())
		{
			send_buffer.clear();
			bytes_sent = 0;
		}

		return n;
	}
	else
	{
		// n <= 0: erreur - fermer la connexion
		return -1;
	}
}

// Verifie si tout le send_buffer a ete envoye
bool Connection::has_pending_data() const
{
	return (!send_buffer.empty() && bytes_sent < send_buffer.length());
}
