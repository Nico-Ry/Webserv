#include "../../include/network/SocketManager.hpp"
#include <cstring>
#include <cerrno>
#include <sstream>

// SocketException
SocketManager::SocketException::SocketException(const std::string& message)
	: std::runtime_error(message) {
}

// SocketManager
SocketManager::SocketManager() {
}

SocketManager::~SocketManager() {
}

int SocketManager::create_socket() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		throw SocketException("socket() failed: " + std::string(strerror(errno)));
	}
	return fd;
}

void SocketManager::configure_socket(int fd) {
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw SocketException("setsockopt(SO_REUSEADDR) failed: " + std::string(strerror(errno)));
	}
}

void SocketManager::bind_socket(int fd, int port) {
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		std::ostringstream oss;
		oss << "bind() failed on port " << port << ": " << strerror(errno);
		throw SocketException(oss.str());
	}
}

void SocketManager::start_listening(int fd, int backlog) {
	if (listen(fd, backlog) < 0) {
		throw SocketException("listen() failed: " + std::string(strerror(errno)));
	}
}

int SocketManager::create_server(int port, int backlog) {
	int fd = create_socket();

	try {
		configure_socket(fd);
		bind_socket(fd, port);
		start_listening(fd, backlog);
	} catch (const SocketException& e) {
		close(fd);
		throw;
	}

	return fd;
}

int SocketManager::accept_connection(int server_fd) {
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	std::memset(&client_addr, 0, sizeof(client_addr));

	int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);

	if (client_fd < 0) {
		throw SocketException("accept() failed: " + std::string(strerror(errno)));
	}

	return client_fd;
}

void SocketManager::close_socket(int fd) {
	if (fd >= 0) {
		close(fd);
	}
}
