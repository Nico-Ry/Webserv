#include "../../include/network/IOMultiplexer.hpp"
#include <cerrno>
#include <cstring>

IOMultiplexer::MultiplexerException::MultiplexerException(const std::string& message)
	: std::runtime_error(message)
{}

IOMultiplexer::IOMultiplexer()
{}

IOMultiplexer::~IOMultiplexer()
{}

void IOMultiplexer::add_fd(int fd, short events)
{
	if (has_fd(fd))
	{
		throw MultiplexerException("fd already exists in multiplexer");
	}

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;

	fds.push_back(pfd);
	fd_to_index[fd] = fds.size() - 1;
}

void IOMultiplexer::remove_fd(int fd)
{
	if (!has_fd(fd)) {
		throw MultiplexerException("fd not found in multiplexer");
	}

	size_t idx = fd_to_index[fd];

	// Swap avec le dernier element
	if (idx < fds.size() - 1) {
		fds[idx] = fds[fds.size() - 1];
		// Mettre a jour l'index du fd deplace
		fd_to_index[fds[idx].fd] = idx;
	}

	fds.pop_back();
	fd_to_index.erase(fd);
}

void IOMultiplexer::modify_fd(int fd, short events)
{
	if (!has_fd(fd)) {
		throw MultiplexerException("fd not found in multiplexer");
	}

	size_t idx = fd_to_index[fd];
	fds[idx].events = events;
	fds[idx].revents = 0;
}

std::vector<int> IOMultiplexer::wait(int timeout)
{
	std::vector<int> result;

	if (fds.empty()) {
		return (result);
	}

	int ready = poll(&fds[0], fds.size(), timeout);

	if (ready < 0)
	{
		throw MultiplexerException("poll() failed: " + std::string(strerror(errno)));
	}

	if (ready == 0)
	{
		// Timeout, pas d'evenements
		return (result);
	}

	// Parcourir les fds et retourner ceux avec des evenements
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].revents != 0) {
			result.push_back(fds[i].fd);
		}
	}

	return (result);
}

short IOMultiplexer::get_revents(int fd) const
{
	std::map<int, size_t>::const_iterator it = fd_to_index.find(fd);
	if (it == fd_to_index.end())
	{
		return (0);
	}
	return (fds[it->second].revents);
}

bool IOMultiplexer::has_fd(int fd) const
{
	return (fd_to_index.find(fd) != fd_to_index.end());
}

size_t IOMultiplexer::size() const
{
	return (fds.size());
}
