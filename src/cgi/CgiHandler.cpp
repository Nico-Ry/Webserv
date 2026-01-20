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

// ============================================================================
// PUBLIC METHODS
// ============================================================================

HttpResponse CgiHandler::execute(const HttpRequest& req,
                                  const std::string& scriptPath,
                                  const std::string& interpreterPath,
                                  int timeout) {
    // DEBUG: Print the script path being tested
    std::cout << "[CGI DEBUG] Testing script path: " << scriptPath << std::endl;
    std::cout << "[CGI DEBUG] access() result: " << access(scriptPath.c_str(), F_OK) << std::endl;
    if (access(scriptPath.c_str(), F_OK) != 0) {
        std::cout << "[CGI DEBUG] errno: " << errno << " (" << strerror(errno) << ")" << std::endl;
    }

    // Verify script exists
    if (access(scriptPath.c_str(), F_OK) != 0) {
        return CgiUtils::generateErrorResponse(404, "CGI script not found: " + scriptPath);
    }

    // Auto-detect interpreter if not provided
    std::string interpreter = interpreterPath;
    if (interpreter.empty()) {
        interpreter = detectInterpreter(scriptPath);
        if (interpreter.empty()) {
            return CgiUtils::generateErrorResponse(500, "Cannot determine interpreter for: " + scriptPath);
        }
    }

    // Verify interpreter exists
    if (access(interpreter.c_str(), X_OK) != 0) {
        return CgiUtils::generateErrorResponse(500, "Interpreter not found or not executable: " + interpreter);
    }

    // Create pipes: [0] = read, [1] = write
    int pipe_in[2];   // For writing to CGI stdin
    int pipe_out[2];  // For reading from CGI stdout

    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) {
        return CgiUtils::generateErrorResponse(500, "Failed to create pipes");
    }

    // Fork child process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
        return CgiUtils::generateErrorResponse(500, "Failed to fork process");
    }

    if (pid == 0) {
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

        // Build environment variables
        std::vector<std::string> env_strings = CgiEnvironment::buildEnvironment(req, scriptPath);

        // Convert to char* array for execve
        std::vector<char*> env_ptrs;
        for (size_t i = 0; i < env_strings.size(); i++) {
            env_ptrs.push_back(const_cast<char*>(env_strings[i].c_str()));
        }
        env_ptrs.push_back(NULL);

        // Build argv: [interpreter, script, NULL]
        char* argv[3];
        argv[0] = const_cast<char*>(interpreter.c_str());
        argv[1] = const_cast<char*>(scriptPath.c_str());
        argv[2] = NULL;

        // Execute CGI script
        execve(interpreter.c_str(), argv, &env_ptrs[0]);

        // If execve returns, it failed
        std::cerr << "CGI Error: execve failed: " << strerror(errno) << std::endl;
        exit(1);
    }

    // ========================================================================
    // PARENT PROCESS - Write request body and read response
    // ========================================================================

    // Close unused pipe ends
    close(pipe_in[0]);   // Parent doesn't read from stdin pipe
    close(pipe_out[1]);  // Parent doesn't write to stdout pipe

    // Write request body to CGI stdin (for POST requests)
    if (!req.body.empty()) {
        ssize_t written = write(pipe_in[1], req.body.c_str(), req.body.size());
        if (written < 0) {
            close(pipe_in[1]);
            close(pipe_out[0]);
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            return CgiUtils::generateErrorResponse(500, "Failed to write request body to CGI");
        }
    }
    close(pipe_in[1]);  // Close stdin pipe (signals EOF to CGI)

    // Read CGI output from stdout
    std::string cgi_output;
    char buffer[4096];
    ssize_t bytes_read;

    // Set timeout for read operations (using select with timeout)
    fd_set read_fds;
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(pipe_out[0], &read_fds);

        int select_ret = select(pipe_out[0] + 1, &read_fds, NULL, NULL, &tv);

        if (select_ret < 0) {
            // Select error
            close(pipe_out[0]);
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            return CgiUtils::generateErrorResponse(500, "Error while reading CGI output");
        }

        if (select_ret == 0) {
            // Timeout
            close(pipe_out[0]);
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            return CgiUtils::generateErrorResponse(504, "CGI script timeout");
        }

        // Data available
        bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            close(pipe_out[0]);
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            return CgiUtils::generateErrorResponse(500, "Error reading CGI output");
        }

        if (bytes_read == 0) {
            // EOF
            break;
        }

        buffer[bytes_read] = '\0';
        cgi_output.append(buffer, bytes_read);
    }

    close(pipe_out[0]);

    // Wait for child process to finish
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        return CgiUtils::generateErrorResponse(500, "Error waiting for CGI process");
    }

    // Check if child exited normally
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        return CgiUtils::generateErrorResponse(502, "CGI script failed to execute");
    }

    // Parse CGI output
    return CgiParser::parseCgiOutput(cgi_output);
}

bool CgiHandler::isCgiScript(const std::string& path) {
    if (path.size() < 3) return false;

    // Check file extensions
    size_t dot_pos = path.rfind('.');
    if (dot_pos == std::string::npos) return false;

    std::string ext = path.substr(dot_pos);
    return (ext == ".py" || ext == ".php" || ext == ".pl" ||
            ext == ".sh" || ext == ".cgi" || ext == ".rb");
}

std::string CgiHandler::detectInterpreter(const std::string& scriptPath) {
    size_t dot_pos = scriptPath.rfind('.');
    if (dot_pos == std::string::npos) return "";

    std::string ext = scriptPath.substr(dot_pos);

    if (ext == ".py") {
        // Try python3 first, then python
        if (access("/usr/bin/python3", X_OK) == 0) return "/usr/bin/python3";
        if (access("/usr/bin/python", X_OK) == 0) return "/usr/bin/python";
    }
    else if (ext == ".php") {
        if (access("/usr/bin/php", X_OK) == 0) return "/usr/bin/php";
        if (access("/usr/bin/php-cgi", X_OK) == 0) return "/usr/bin/php-cgi";
    }
    else if (ext == ".pl") {
        if (access("/usr/bin/perl", X_OK) == 0) return "/usr/bin/perl";
    }
    else if (ext == ".sh") {
        if (access("/bin/bash", X_OK) == 0) return "/bin/bash";
        if (access("/bin/sh", X_OK) == 0) return "/bin/sh";
    }
    else if (ext == ".rb") {
        if (access("/usr/bin/ruby", X_OK) == 0) return "/usr/bin/ruby";
    }

    return "";
}