#include "../../include/cgi/CgiHandler.hpp"
#include "../../include/cgi/CgiEnvironment.hpp"
#include "../../include/cgi/CgiParser.hpp"
#include "../../include/cgi/CgiUtils.hpp"
#include "../../include/http/Status.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

// Helper: set fd to non-blocking mode
// Note: subject only allows F_SETFL, O_NONBLOCK, FD_CLOEXEC (no F_GETFL)
static bool setNonBlocking(int fd)
{
	return fcntl(fd, F_SETFL, O_NONBLOCK) >= 0;
}

// ============================================================================
// PUBLIC METHODS
// ============================================================================

CgiProcess* CgiHandler::startCgi(const HttpRequest& req,
								 const std::string& scriptPath,
								 int client_fd,
								 const std::string& interpreterPath,
								 int timeout)
{
	// Verify script exists
	if (access(scriptPath.c_str(), F_OK) != 0)
	{
		std::cerr << "[CGI] Script not found: " << scriptPath << std::endl;
		return NULL;
	}

	// Auto-detect interpreter if not provided
	std::string interpreter = interpreterPath;
	if (interpreter.empty())
	{
		interpreter = detectInterpreter(scriptPath);
		if (interpreter.empty())
		{
			std::cerr << "[CGI] Cannot determine interpreter for: " << scriptPath << std::endl;
			return NULL;
		}
	}

	// Verify interpreter exists
	if (access(interpreter.c_str(), X_OK) != 0)
	{
		std::cerr << "[CGI] Interpreter not found: " << interpreter << std::endl;
		return NULL;
	}

	// Create pipes: [0] = read, [1] = write
	int pipe_in[2];   // For writing to CGI stdin
	int pipe_out[2];  // For reading from CGI stdout

	if (pipe(pipe_in) < 0)
	{
		std::cerr << "[CGI] Failed to create pipe_in" << std::endl;
		return NULL;
	}
	if (pipe(pipe_out) < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		std::cerr << "[CGI] Failed to create pipe_out" << std::endl;
		return NULL;
	}

	// Fork child process
	pid_t pid = fork();

	if (pid < 0)
	{
		// Fork failed
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		std::cerr << "[CGI] Fork failed" << std::endl;
		return NULL;
	}

	if (pid == 0)
	{
		// ====================================================================
		// CHILD PROCESS - Execute CGI script
		// ====================================================================

		// Redirect stdin to pipe_in (for POST body)
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[0]);
		close(pipe_in[1]);

		// Redirect stdout to pipe_out (for CGI output)
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[0]);
		close(pipe_out[1]);

		// Change to script directory for relative path file access (required by subject)
		std::string scriptName = scriptPath;
		size_t lastSlash = scriptPath.rfind('/');
		if (lastSlash != std::string::npos)
		{
			std::string scriptDir = scriptPath.substr(0, lastSlash);
			scriptName = scriptPath.substr(lastSlash + 1);
			if (!scriptDir.empty())
			{
				chdir(scriptDir.c_str());
			}
		}

		// Build environment variables
		std::vector<std::string> env_strings = CgiEnvironment::buildEnvironment(req, scriptPath);

		// Convert to char* array for execve
		std::vector<char*> env_ptrs;
		for (size_t i = 0; i < env_strings.size(); i++)
		{
			env_ptrs.push_back(const_cast<char*>(env_strings[i].c_str()));
		}
		env_ptrs.push_back(NULL);

		// Build argv: [interpreter, script, NULL]
		char* argv[3];
		argv[0] = const_cast<char*>(interpreter.c_str());
		argv[1] = const_cast<char*>(scriptName.c_str());
		argv[2] = NULL;

		// Execute CGI script
		execve(interpreter.c_str(), argv, &env_ptrs[0]);

		// If execve returns, it failed
		std::cerr << "CGI Error: execve failed: " << strerror(errno) << std::endl;
		_exit(1);
	}

	// ========================================================================
	// PARENT PROCESS - Return immediately (non-blocking)
	// ========================================================================

	// Close unused pipe ends
	close(pipe_in[0]);   // Parent doesn't read from stdin pipe
	close(pipe_out[1]);  // Parent doesn't write to stdout pipe

	// Set pipes to non-blocking mode
	if (!setNonBlocking(pipe_in[1]) || !setNonBlocking(pipe_out[0]))
	{
		close(pipe_in[1]);
		close(pipe_out[0]);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, WNOHANG);  // Non-blocking reap
		std::cerr << "[CGI] Failed to set pipes non-blocking" << std::endl;
		return NULL;
	}

	// Create CgiProcess structure
	CgiProcess* cgi = new CgiProcess();
	cgi->pid = pid;
	cgi->pipe_in = pipe_in[1];
	cgi->pipe_out = pipe_out[0];
	cgi->client_fd = client_fd;
	cgi->body = req.body;
	cgi->body_written = 0;
	cgi->output = "";
	cgi->start_time = time(NULL);
	cgi->timeout = timeout;

	// If no body to write, start in reading state and close pipe_in
	if (cgi->body.empty())
	{
		close(cgi->pipe_in);
		cgi->pipe_in = -1;
		cgi->state = CgiProcess::CGI_READING_OUTPUT;
	}
	else
	{
		cgi->state = CgiProcess::CGI_WRITING_BODY;
	}

	// std::cout << "[CGI] Started async CGI: pid=" << pid
	// 		  << " client_fd=" << client_fd
	// 		  << " script=" << scriptPath << std::endl;

	return cgi;
}

// NOTE: CgiHandler::execute() was removed - it was a blocking implementation
// that violated the subject requirement "server must remain non-blocking at all times"
// and "only 1 poll() for all I/O". Use executeAsync() instead.

bool CgiHandler::isCgiScript(const std::string& path)
{
	if (path.size() < 3)
		return (false);

	// Check file extensions
	size_t dot_pos = path.rfind('.');
	if (dot_pos == std::string::npos)
		return (false);

	std::string ext = path.substr(dot_pos);
	return (ext == ".py" || ext == ".php" || ext == ".pl" ||
			ext == ".sh" || ext == ".cgi" || ext == ".rb");
}

std::string CgiHandler::detectInterpreter(const std::string& scriptPath)
{
	size_t dot_pos = scriptPath.rfind('.');
	if (dot_pos == std::string::npos)
		return ("");

	std::string ext = scriptPath.substr(dot_pos);

	if (ext == ".py")
	{
		// Try python3 first, then python
		if (access("/usr/bin/python3", X_OK) == 0)
			return ("/usr/bin/python3");
		if (access("/usr/bin/python", X_OK) == 0)
			return ("/usr/bin/python");
	}
	else if (ext == ".php")
	{
		if (access("/usr/bin/php", X_OK) == 0)
			return ("/usr/bin/php");
		if (access("/usr/bin/php-cgi", X_OK) == 0)
			return ("/usr/bin/php-cgi");
	}
	else if (ext == ".pl")
	{
		if (access("/usr/bin/perl", X_OK) == 0)
			return ("/usr/bin/perl");
	}
	else if (ext == ".sh")
	{
		if (access("/bin/bash", X_OK) == 0)
			return ("/bin/bash");
		if (access("/bin/sh", X_OK) == 0)
			return ("/bin/sh");
	}
	else if (ext == ".rb")
	{
		if (access("/usr/bin/ruby", X_OK) == 0)
			return ("/usr/bin/ruby");
	}

	return ("");
}
