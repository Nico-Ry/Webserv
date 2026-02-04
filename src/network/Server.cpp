#include "network/Server.hpp"
#include "http/Status.hpp"
#include "router/Router.hpp"
#include "cgi/CgiHandler.hpp"
#include "cgi/CgiParser.hpp"
#include "colours.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <ctime>
#include <sys/wait.h>
#include <csignal>
#include <unistd.h>

// Timeout pour les connexions clients inactives (en secondes)
static const int CLIENT_TIMEOUT_SECONDS = 30;

// Helper function pour convertir int en string (C++98)
std::string intToString(int n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

Server::ServerException::ServerException(const std::string& message)
	: std::runtime_error(message)
{}

Server::Server(const Config& cfg, int backlog)
	: socket_manager(), multiplexer(), clients(), config(&cfg), running(false)
	{

	std::cout << BOLD_CYAN << "=== Initializing Multi-Port Server ===" << RES << std::endl;

	if (cfg.servers.empty())
	{
		throw ServerException("No server blocks found in configuration");
	}

	// Creer un socket pour chaque ServerBlock dans la config
	try {
		for (size_t i = 0; i < cfg.servers.size(); i++)
		{
			int port = cfg.servers[i].port;

			std::cout << BOLD_CYAN << "Setting up server on port " << port << "..." << RES << std::endl;

			// Creer le socket serveur
			int fd = socket_manager.create_server(port, backlog);

			// Stocker le fd et sa config associée
			server_fds.push_back(fd);
			fd_to_server[fd] = &cfg.servers[i];

			// Ajouter au multiplexer pour surveiller les nouvelles connexions
			multiplexer.add_fd(fd, POLLIN);

			std::cout << GREEN << "✓ " << RES << "Server socket created: "
					  << "fd=" << fd << " (port " << port << ")" << std::endl;
		}
	} catch (...)
	{
		// Nettoyer les sockets deja crees en cas d'echec
		for (size_t i = 0; i < server_fds.size(); i++)
		{
			SocketManager::close_socket(server_fds[i]);
		}
		server_fds.clear();
		fd_to_server.clear();
		throw;
	}

	std::cout << GREEN << "✓ " << RES << "All servers ready to accept connections ("
			  << server_fds.size() << " port" << (server_fds.size() > 1 ? "s" : "") << ")" << std::endl;
}

Server::~Server()
{
	std::cout << BOLD_ORANGE << "=== Shutting down server ===" << RES << std::endl;

	// Nettoyer tous les CGI en cours
	for (std::map<int, CgiProcess*>::iterator it = cgi_by_pipe_out.begin();
		 it != cgi_by_pipe_out.end(); ++it)
	{
		CgiProcess* cgi = it->second;
		if (cgi->pipe_in >= 0)
			close(cgi->pipe_in);
		if (cgi->pipe_out >= 0)
			close(cgi->pipe_out);
		if (cgi->pid > 0)
		{
			kill(cgi->pid, SIGKILL);
			waitpid(cgi->pid, NULL, WNOHANG);  // Non-blocking reap
		}
		delete cgi;
	}
	cgi_by_pipe_in.clear();
	cgi_by_pipe_out.clear();
	cgi_by_client.clear();

	// Fermer toutes les connexions clients
	for (std::map<int, Connection*>::iterator it = clients.begin();
		 it != clients.end(); ++it)
	{
		delete it->second;
	}
	clients.clear();

	// Liberer tous les parsers HTTP
	for (std::map<int, HttpRequestParser*>::iterator it = parsers.begin();
		 it != parsers.end(); ++it)
	{
		delete it->second;
	}
	parsers.clear();

	// Fermer tous les sockets serveurs
	for (size_t i = 0; i < server_fds.size(); i++)
	{
		if (server_fds[i] >= 0)
		{
			SocketManager::close_socket(server_fds[i]);
		}
	}
	server_fds.clear();
	fd_to_server.clear();

	std::cout << GREEN << "✓ " << RES << "Server stopped" << std::endl;
}

void Server::run()
{
	running = true;

	std::cout << std::endl << "Multi-client server running on ports: ";
	for (size_t i = 0; i < server_fds.size(); i++) {
		const ServerBlock* sb = fd_to_server[server_fds[i]];
		std::cout << GREEN << sb->port << RES;
		if (i < server_fds.size() - 1) std::cout << ", ";
	}
	std::cout << std::endl << "(Ctrl+C to stop)\n" << std::endl;

	while (running)
	{
		// Attendre des evenements sur les fds surveilles (timeout 5s pour vérifier les timeouts)
		std::vector<int> ready_fds = multiplexer.wait(5000);

		// Vérifier les connexions inactives et les CGI timeouts
		checkClientTimeouts();
		checkCgiTimeouts();

		if (ready_fds.empty()) {
			continue;
		}

		// Traiter chaque fd pret
		for (size_t i = 0; i < ready_fds.size(); i++)
		{
			int fd = ready_fds[i];

			// Verifier si c'est un server socket (nouvelle connexion)
			if (isServerSocket(fd))
			{
				acceptNewClient(fd);
			}
			// Verifier si c'est un pipe CGI (stdin pour ecriture)
			else if (cgi_by_pipe_in.find(fd) != cgi_by_pipe_in.end())
			{
				short revents = multiplexer.get_revents(fd);
				if (revents & POLLOUT)
				{
					handleCgiWrite(fd);
				}
			}
			// Verifier si c'est un pipe CGI (stdout pour lecture)
			else if (cgi_by_pipe_out.find(fd) != cgi_by_pipe_out.end())
			{
				short revents = multiplexer.get_revents(fd);
				if (revents & (POLLIN | POLLHUP))
				{
					handleCgiRead(fd);
				}
			}
			// Sinon c'est un client socket
			else if (clients.find(fd) != clients.end())
			{
				short revents = multiplexer.get_revents(fd);
				std::cout << "[DEBUG] Client fd=" << fd << " revents: "
						  << ((revents & POLLIN) ? "POLLIN " : "")
						  << ((revents & POLLOUT) ? "POLLOUT " : "")
						  << ((revents & POLLHUP) ? "POLLHUP " : "")
						  << ((revents & POLLERR) ? "POLLERR " : "")
						  << "pending_data=" << clients[fd]->has_pending_data() << std::endl;

				// Verifier d'abord les erreurs poll (connexion fermee, erreur socket)
				if (revents & (POLLERR | POLLNVAL))
				{
					removeClient(fd);
					continue;
				}

				// POLLHUP = client a ferme la connexion, mais il peut rester des donnees a lire
				// On essaie de lire d'abord si POLLIN est aussi present
				bool client_disconnected = false;

				if (revents & POLLIN)
				{
					handleClientRead(fd);

					// Verifier si le client a ete supprime pendant la lecture
					if (clients.find(fd) == clients.end())
					{
						client_disconnected = true;
					}
				}

				// IMPORTANT: traiter POLLOUT meme si POLLHUP (half-close du client)
				// On doit envoyer la reponse avant de fermer
				if (!client_disconnected && (revents & POLLOUT))
				{
					std::cout << "[DEBUG] POLLOUT ready for client fd=" << fd
							  << " send_buffer size=" << clients[fd]->send_buffer.size() << std::endl;
					handleClientWrite(fd);
					// Verifier si client existe encore apres write
					if (clients.find(fd) == clients.end())
						continue;
				}

				// Si POLLHUP sans POLLIN et plus de donnees a envoyer, fermer
				if (!client_disconnected && (revents & POLLHUP) && !(revents & POLLIN))
				{
					// Ne fermer que si on n'a plus rien a envoyer
					if (!clients[fd]->has_pending_data())
					{
						removeClient(fd);
						continue;
					}
					// Sinon on garde la connexion pour finir d'envoyer
				}
			}
		}

		std::cout << std::endl;
	}
}

void Server::stop()
{
	running = false;
}

bool Server::isServerSocket(int fd) const
{
	return (fd_to_server.find(fd) != fd_to_server.end());
}

void Server::acceptNewClient(int server_fd)
{
	try {
		int client_fd = socket_manager.accept_connection(server_fd);
		Connection* conn = new Connection(client_fd);
		clients[client_fd] = conn;

		// Stocker quel ServerBlock a accepté ce client
		const ServerBlock* sb = fd_to_server.find(server_fd)->second;
		client_to_server[client_fd] = sb;

		// Surveiller le client pour les donnees entrantes
		multiplexer.add_fd(client_fd, POLLIN);
		std::cout << GREEN << "✓ " << RES << "New client connected on port " << sb->port << ": "
				  << "fd=" << client_fd << " -> (total clients: " << clients.size() << ")"
				  << std::endl;
	}
	catch (const SocketManager::SocketException& e)
	{
		std::cerr << "✗ Error accepting client: " << e.what() << std::endl;
	}
	catch (const Connection::ConnectionException& e)
	{
		std::cerr << "✗ Error creating connection: " << e.what() << std::endl;
	}
}

void Server::handleClientRead(int fd)
{
	Connection* conn = clients[fd];
	ssize_t n = conn->read_available();

	if (n > 0 || n == -2)
	{
		// Mettre à jour le timestamp d'activité
		conn->update_activity();

		// std::cout << "  [fd=" << fd << "] Received " << n << " bytes" << std::endl;
		// std::cout << BOLD_YELLOW << "[DEBUG] " << RES << "Received " << n << " bytes" << std::endl;

		// Traiter la requete HTTP avec le parser
		processRequest(conn, fd);

		// Activer POLLOUT si une reponse est prete
		if (!conn->send_buffer.empty())
		{
			multiplexer.modify_fd(fd, POLLIN | POLLOUT);
		}
	}
	else if (n == 0)
	{
		// Client a ferme la connexion (ou half-close avec nc -N)
		// Ne pas fermer si on a une reponse a envoyer (half-close scenario)
		if (conn->has_pending_data())
		{
			std::cout << "[DEBUG] Client half-closed but we have data to send" << std::endl;
			// Desactiver POLLIN, garder POLLOUT pour envoyer la reponse
			multiplexer.modify_fd(fd, POLLOUT);
			conn->should_close = true;  // Fermer apres envoi
		}
		else
		{
			// Pas de donnees a envoyer, fermer maintenant
			removeClient(fd);
		}
	}
	// else if (n == -2) {
	// 	conn->send_buffer = ResponseBuilder::build(HttpResponse(413, "Payload Too Large"), true);
	// }
	else
	{
		// Erreur de lecture - fermer la connexion silencieusement
		// (normal sous haute charge quand le client ferme la connexion)
		removeClient(fd);
	}
}

void Server::handleClientWrite(int fd)
{
	Connection* conn = clients[fd];

	std::cout << "[DEBUG] handleClientWrite fd=" << fd
			  << " has_pending=" << conn->has_pending_data()
			  << " buffer_size=" << conn->send_buffer.size()
			  << " bytes_sent=" << conn->bytes_sent << std::endl;

	if (conn->has_pending_data())
	{
		ssize_t sent = conn->write_pending();
		std::cout << "[DEBUG] write_pending returned " << sent << std::endl;

		if (sent > 0)
		{
			std::cout << "[DEBUG] Sent " << sent << " bytes, remaining="
					  << (conn->send_buffer.size() - conn->bytes_sent) << std::endl;
		}
		else if (sent < 0)
		{
			std::cout << "[DEBUG] write_pending error, removing client" << std::endl;
			// Erreur d'ecriture - fermer silencieusement
			removeClient(fd);
			return ;
		}

		// Si tout a ete envoye
		if (!conn->has_pending_data())
		{
			std::cout << "[DEBUG] All data sent for fd=" << fd << std::endl;

			// Pipelining: verifier si le parser a des donnees bufferisees (prochaine requete)
			// IMPORTANT: faire ceci AVANT de fermer la connexion (meme si should_close)
			std::map<int, HttpRequestParser*>::iterator parser_it = parsers.find(fd);
			if (parser_it != parsers.end() && parser_it->second->hasBufferedData())
			{
				std::cout << "[DEBUG] Pipelining: processing next buffered request" << std::endl;
				processRequest(conn, fd);
				// Verifier si client existe encore
				if (clients.find(fd) == clients.end())
					return;
				// Si une nouvelle reponse est prete, activer POLLOUT
				if (!conn->send_buffer.empty())
				{
					multiplexer.modify_fd(fd, POLLOUT);
					return;
				}
			}

			// Fermer si Connection: close etait demande (ou half-close)
			if (conn->should_close)
			{
				removeClient(fd);
				return;
			}

			// Sinon garder la connexion (keep-alive)
			multiplexer.modify_fd(fd, POLLIN);
		}
	}
	else
	{
		std::cout << "[DEBUG] No pending data for fd=" << fd << std::endl;
		// Plus rien a envoyer, desactiver POLLOUT
		multiplexer.modify_fd(fd, POLLIN);
	}
}

void Server::removeClient(int fd)
{
	std::map<int, Connection*>::iterator it = clients.find(fd);

	if (it != clients.end())
	{
		// Cleanup any CGI running for this client
		std::map<int, CgiProcess*>::iterator cgi_it = cgi_by_client.find(fd);
		if (cgi_it != cgi_by_client.end())
		{
			std::cout << "[CGI] Client fd=" << fd << " disconnecting, cleaning up CGI" << std::endl;
			cleanupCgi(cgi_it->second);
		}

		multiplexer.remove_fd(fd);
		delete it->second;
		clients.erase(it);

		// Supprimer aussi le parser HTTP associe
		std::map<int, HttpRequestParser*>::iterator parser_it = parsers.find(fd);
		if (parser_it != parsers.end())
		{
			delete parser_it->second;
			parsers.erase(parser_it);
		}

		// Supprimer le mapping client -> server
		client_to_server.erase(fd);
	}
}

void Server::processRequest(Connection* conn, int fd)
{
	// Catches casses where bytes read exceeds limit set in Connection.hpp
	if (conn->totalBytesReceived > conn->maxRequestSize) {
		HttpResponse resp(413, "Payload Too Large");
		conn->send_buffer = ResponseBuilder::build(resp, true);
		printNonSuccess(resp);
		return;
	}

	// Creer un parser pour ce client si necessaire
	if (parsers.find(fd) == parsers.end())
	{
		parsers[fd] = new HttpRequestParser();
		// std::cout << "  [fd=" << fd << "] Created HTTP parser" << std::endl;
	}

	HttpRequestParser* parser = parsers[fd];

	// Envoyer les donnees au parser
	// std::cout << BOLD_GOLD << conn->recv_buffer << RES << std::endl;
	parser->feed(conn->recv_buffer);
	conn->recv_buffer.clear();
		// Prevent pipelining from overwriting the pending response
	if (!conn->send_buffer.empty())
		return;

	if (parser->hasError())
	{
		// Generer une reponse d'erreur HTTP
		int errorCode = parser->getErrorStatus();
		HttpResponse resp(errorCode, reasonPhrase(errorCode));


	// Build HTML Error Page
		resp.headers["Content-Type"] = "text/html";
		resp.body = generateErrorHtml(resp.statusCode, resp.reason);
		printNonSuccess(resp);

		// resp.headers["Content-Length: "] = toStringSize(resp.body.size());//NICO DID THIS CHANGE


		// Toujours fermer connexion sur erreur
		conn->send_buffer = ResponseBuilder::build(resp, true);
				// IMPORTANT: reset parser also on error
		if (parser->hasBufferedData())
			parser->resetKeepBuffer();
		else
			parser->reset();

		return;
	}

	// Verifier si la requete est complete
	else if (parser->isDone())
	{
		// Traiter la requete HTTP valide
		const HttpRequest& req = parser->getRequest();

		// Generer la reponse HTTP avec le bon ServerBlock
		const ServerBlock* serverBlock = client_to_server[fd];
		Router	requestHandler(*config, serverBlock);
		HttpResponse resp = requestHandler.buildResponse(req);

		// Check if this is a CGI request that needs async execution
		if (resp.isCgiPending)
		{
			// Verify client doesn't already have a CGI running
			if (cgi_by_client.find(fd) != cgi_by_client.end())
			{
				std::cerr << "[CGI] Client fd=" << fd << " already has CGI running" << std::endl;
				HttpResponse errResp(503, "Service Unavailable");
				errResp.body = "CGI already running for this client";
				conn->send_buffer = ResponseBuilder::build(errResp, true);
			}
			else
			{
				// Start CGI asynchronously
				CgiProcess* cgi = CgiHandler::startCgi(req, resp.cgiScriptPath, fd);
				if (cgi == NULL)
				{
					// CGI failed to start
					HttpResponse errResp(500, "Internal Server Error");
					errResp.body = "Failed to start CGI";
					conn->send_buffer = ResponseBuilder::build(errResp, true);
				}
				else
				{
					// Save connection close preference (HTTP/1.0 vs 1.1)
					cgi->should_close = parser->shouldCloseConnection();

					// Register CGI pipes in poll()
					if (cgi->pipe_in >= 0)
					{
						multiplexer.add_fd(cgi->pipe_in, POLLOUT);
						cgi_by_pipe_in[cgi->pipe_in] = cgi;
					}
					multiplexer.add_fd(cgi->pipe_out, POLLIN);
					cgi_by_pipe_out[cgi->pipe_out] = cgi;
					cgi_by_client[fd] = cgi;

					// Don't send response yet - wait for CGI to complete
					// Reset parser for next request
					if (parser->hasBufferedData())
						parser->resetKeepBuffer();
					else
						parser->reset();
					return;
				}
			}
		}
		else
		{
			// Normal response (not CGI)
			bool closeConnection = parser->shouldCloseConnection();
			conn->send_buffer = ResponseBuilder::build(resp, closeConnection);
			conn->should_close = closeConnection;  // Fermer apres envoi si demande

			std::cout	<< std::left << BOLD_BLACK << std::setw(16) << "[Server]" << RES << "  ~  (Connection: "
						<< (closeConnection ? "close" : "keep-alive") << ")" << std::endl;
		}

		// Reset parser pour la prochaine requete (keep-alive)
		if (parser->hasBufferedData())
			parser->resetKeepBuffer();
		else
			parser->reset();
	}
}

void Server::checkClientTimeouts() {
	time_t now = time(NULL);
	std::vector<int> to_remove;

	// Identifier les clients qui ont dépassé le timeout
	for (std::map<int, Connection*>::iterator it = clients.begin();
		 it != clients.end(); ++it) {
		int fd = it->first;
		Connection* conn = it->second;

		// Ne pas timeout les clients qui ont un CGI en cours
		if (cgi_by_client.find(fd) != cgi_by_client.end())
			continue;

		if (now - conn->last_activity > CLIENT_TIMEOUT_SECONDS) {
			std::cout << BOLD_ORANGE << "[TIMEOUT] " << RES
				<< "Client fd=" << fd << " inactive for "
				<< (now - conn->last_activity) << "s, closing connection" << std::endl;
			to_remove.push_back(fd);
		}
	}

	// Fermer les connexions timeout (on ne peut pas modifier la map pendant l'itération)
	for (size_t i = 0; i < to_remove.size(); ++i) {
		removeClient(to_remove[i]);
	}
}

// ============================================================================
// CGI NON-BLOQUANT
// ============================================================================

bool Server::isCgiPipe(int fd) const
{
	return (cgi_by_pipe_in.find(fd) != cgi_by_pipe_in.end() ||
			cgi_by_pipe_out.find(fd) != cgi_by_pipe_out.end());
}

void Server::handleCgiWrite(int pipe_fd)
{
	std::map<int, CgiProcess*>::iterator it = cgi_by_pipe_in.find(pipe_fd);
	if (it == cgi_by_pipe_in.end())
		return;

	CgiProcess* cgi = it->second;

	// Write body to CGI stdin (one write per poll event)
	if (cgi->hasBodyToWrite())
	{
		const char* data = cgi->body.c_str() + cgi->body_written;
		size_t remaining = cgi->body.size() - cgi->body_written;

		ssize_t n = write(pipe_fd, data, remaining);
		if (n > 0)
		{
			cgi->body_written += n;
			std::cout << "[CGI] Wrote " << n << " bytes to CGI stdin (total: "
					  << cgi->body_written << "/" << cgi->body.size() << ")" << std::endl;
		}
		else if (n < 0)
		{
			// Subject forbids checking errno after write()
			// Treat as temporary failure, retry on next POLLOUT
			// If it's a real error, POLLHUP/POLLERR will be signaled by poll()
			return;
		}
	}

	// If all body written, close pipe_in and switch to reading
	if (!cgi->hasBodyToWrite())
	{
		multiplexer.remove_fd(pipe_fd);
		close(pipe_fd);
		cgi_by_pipe_in.erase(pipe_fd);
		cgi->pipe_in = -1;
		cgi->state = CgiProcess::CGI_READING_OUTPUT;
		std::cout << "[CGI] Finished writing body, now reading output" << std::endl;
	}
}

void Server::handleCgiRead(int pipe_fd)
{
	std::map<int, CgiProcess*>::iterator it = cgi_by_pipe_out.find(pipe_fd);
	if (it == cgi_by_pipe_out.end())
		return;

	CgiProcess* cgi = it->second;
	char buffer[4096];

	// Read from CGI stdout (one read per poll event)
	ssize_t n = read(pipe_fd, buffer, sizeof(buffer));

	if (n > 0)
	{
		cgi->output.append(buffer, n);
		std::cout << "[CGI] Read " << n << " bytes from CGI (total: "
				  << cgi->output.size() << " bytes)" << std::endl;
	}
	else if (n == 0)
	{
		// EOF - CGI finished output
		std::cout << "[CGI] CGI output complete (" << cgi->output.size() << " bytes)" << std::endl;
		cgi->state = CgiProcess::CGI_DONE;
		finishCgi(cgi);
	}
	else
	{
		// Read error
		std::cerr << "[CGI] Error reading from CGI stdout" << std::endl;
		cgi->state = CgiProcess::CGI_ERROR;
		finishCgi(cgi);
	}
}

void Server::checkCgiTimeouts()
{
	std::vector<CgiProcess*> to_cleanup;

	// Check all active CGI processes
	for (std::map<int, CgiProcess*>::iterator it = cgi_by_pipe_out.begin();
		 it != cgi_by_pipe_out.end(); ++it)
	{
		CgiProcess* cgi = it->second;
		if (cgi->isTimedOut())
		{
			std::cout << BOLD_ORANGE << "[CGI TIMEOUT] " << RES
					  << "CGI pid=" << cgi->pid << " timed out after "
					  << cgi->timeout << "s" << std::endl;
			cgi->state = CgiProcess::CGI_ERROR;
			to_cleanup.push_back(cgi);
		}
	}

	// Cleanup timed out CGIs
	for (size_t i = 0; i < to_cleanup.size(); ++i)
	{
		finishCgi(to_cleanup[i]);
	}
}

void Server::finishCgi(CgiProcess* cgi)
{
	int client_fd = cgi->client_fd;

	// Check if client still exists
	if (clients.find(client_fd) == clients.end())
	{
		std::cerr << "[CGI] Client fd=" << client_fd << " disconnected, discarding CGI output" << std::endl;
		cleanupCgi(cgi);
		return;
	}

	Connection* conn = clients[client_fd];
	HttpResponse resp(500, "Internal Server Error");

	if (cgi->state == CgiProcess::CGI_DONE)
	{
		// Wait for child process with WNOHANG (non-blocking)
		int status;
		pid_t result = waitpid(cgi->pid, &status, WNOHANG);

		// EOF on pipe means CGI finished output - use it regardless of process state
		// result == 0: process still running (will be killed in cleanupCgi)
		// result > 0: process exited, check exit status
		bool cgi_success = false;

		if (result == 0)
		{
			// Process still running but we got EOF - consider it successful
			cgi_success = true;
		}
		else if (result > 0)
		{
			// Process exited - check if it was clean exit with status 0
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
				cgi_success = true;
			else
				std::cerr << "[CGI] CGI process exited with error status" << std::endl;
		}
		else
		{
			// waitpid error
			std::cerr << "[CGI] waitpid error" << std::endl;
		}

		if (cgi_success && !cgi->output.empty())
		{
			// Parse CGI output
			resp = CgiParser::parseCgiOutput(cgi->output);
			std::cout << "[CGI] CGI completed successfully, status=" << resp.statusCode << std::endl;
		}
		else
		{
			resp = HttpResponse(502, "Bad Gateway");
			resp.body = "CGI script failed to execute properly";
			std::cerr << "[CGI] CGI process failed" << std::endl;
		}
	}
	else if (cgi->state == CgiProcess::CGI_ERROR)
	{
		if (cgi->isTimedOut())
		{
			resp = HttpResponse(504, "Gateway Timeout");
			resp.body = "CGI script timed out";
		}
		else
		{
			resp = HttpResponse(502, "Bad Gateway");
			resp.body = "CGI script error";
		}
	}

	// Send response to client (respect HTTP/1.0 vs 1.1 connection handling)
	conn->send_buffer = ResponseBuilder::build(resp, cgi->should_close);
	conn->should_close = cgi->should_close;
	conn->update_activity();  // Reset timeout pour laisser le temps d'envoyer la reponse
	multiplexer.modify_fd(client_fd, POLLIN | POLLOUT);
	std::cout << "[DEBUG] finishCgi: set POLLOUT for client_fd=" << client_fd
			  << " send_buffer size=" << conn->send_buffer.size()
			  << " should_close=" << cgi->should_close << std::endl;

	std::cout << std::left << BOLD_MAGENTA << std::setw(16) << "[HTTP Response]" << RES << "  ~  ["
			  << (resp.statusCode < 400 ? BOLD_GREEN : BOLD_RED) << resp.statusCode << RES << "] ["
			  << (resp.statusCode < 400 ? BOLD_GREEN : BOLD_RED) << resp.reason << RES << "]" << std::endl;

	// Cleanup CGI resources
	cleanupCgi(cgi);
}

void Server::cleanupCgi(CgiProcess* cgi)
{
	// Remove from maps
	if (cgi->pipe_in >= 0)
	{
		multiplexer.remove_fd(cgi->pipe_in);
		cgi_by_pipe_in.erase(cgi->pipe_in);
		close(cgi->pipe_in);
	}
	if (cgi->pipe_out >= 0)
	{
		multiplexer.remove_fd(cgi->pipe_out);
		cgi_by_pipe_out.erase(cgi->pipe_out);
		close(cgi->pipe_out);
	}
	cgi_by_client.erase(cgi->client_fd);

	// Kill process if still running (non-blocking only)
	if (cgi->pid > 0)
	{
		int status;
		pid_t result = waitpid(cgi->pid, &status, WNOHANG);
		if (result == 0)
		{
			// Process still running, kill it
			kill(cgi->pid, SIGKILL);
			// WNOHANG: never block - zombie will be reaped by kernel eventually
			waitpid(cgi->pid, &status, WNOHANG);
		}
	}

	delete cgi;
}
