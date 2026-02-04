#include "../../include/network/Connection.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>

Connection::ConnectionException::ConnectionException(const std::string& message)
	: std::runtime_error(message)
{}

Connection::Connection(int fd)
	: fd(fd), recv_buffer(), send_buffer(), bytes_sent(0), last_activity(time(NULL)), should_close(false)
{
	set_nonblocking();
}

void Connection::update_activity()
{
	last_activity = time(NULL);
}

Connection::~Connection()
{
	if (fd >= 0)
	{
		close(fd);
	}
}

void Connection::set_nonblocking()
{
	// Note: subject only allows F_SETFL, O_NONBLOCK, FD_CLOEXEC (no F_GETFL)
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		throw ConnectionException("fcntl(F_SETFL, O_NONBLOCK) failed: " + std::string(strerror(errno)));
	}
}

ssize_t Connection::read_available()
{
	char buffer[4096];

	// Un seul appel recv() par evenement POLLIN
	// On ne verifie JAMAIS errno apres recv() (interdit par le sujet)
	ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

	if (n > 0)
	{
		recv_buffer.append(buffer, n);
		return n;
	}
	else if (n == 0)
	{
		// Connexion fermee par le client
		return 0;
	}
	else
	{
		// n < 0: erreur - fermer la connexion
		return -1;
	}
}

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

bool Connection::has_pending_data() const
{
	return (!send_buffer.empty() && bytes_sent < send_buffer.length());
}
