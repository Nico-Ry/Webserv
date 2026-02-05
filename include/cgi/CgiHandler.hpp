#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../http/Request.hpp"
#include "../http/HttpResponse.hpp"
#include "../configParser/LocationBlock.hpp"
#include "CgiProcess.hpp"
#include <string>
#include <vector>
#include <map>

/**
 * @brief CGI Handler - Executes CGI scripts and captures their output
 *
 * Supports Python, PHP, Perl, and other CGI scripts.
 * Implements the CGI/1.1 specification.
 *
 * Features:
 * - Fork/exec to run scripts in child process
 * - Environment variables (REQUEST_METHOD, PATH_INFO, QUERY_STRING, etc.)
 * - Stdin/stdout piping for request/response body
 * - Timeout handling (default: 30 seconds)
 * - Error handling (500, 502, 504)
 */
class CgiHandler {
public:
	/**
	 * @brief Start a CGI script asynchronously (non-blocking)
	 *
	 * Forks the process and returns immediately. The caller must:
	 * - Register pipes in poll()
	 * - Handle POLLOUT on pipe_in to write body
	 * - Handle POLLIN on pipe_out to read output
	 * - Call waitpid() with WNOHANG periodically
	 *
	 * @param req The HTTP request
	 * @param scriptPath Full path to the CGI script
	 * @param client_fd The client socket waiting for response
	 * @param interpreterPath Path to interpreter (empty for auto-detect)
	 * @param timeout Maximum execution time in seconds
	 * @return CgiProcess* Process info, or NULL on immediate failure
	 */
	static CgiProcess* startCgi(const HttpRequest& req,
								const std::string& scriptPath,
								int client_fd,
								const std::string& interpreterPath = "",
								int timeout = 10);

	// NOTE: execute() was removed - violated "non-blocking at all times" requirement

	/**
	 * @brief Check if a file is a CGI script based on extension
	 *
	 * @param path File path
	 * @return true if it's a CGI script (.py, .php, .pl, .sh, .cgi)
	 */
	static bool isCgiScript(const std::string& path);

	/**
	 * @brief Auto-detect interpreter from file extension
	 *
	 * @param scriptPath Path to script
	 * @return std::string Path to interpreter, or empty if not found
	 */
	static std::string detectInterpreter(const std::string& scriptPath);

private:
	/**
	 * @brief Build CGI environment variables from HTTP request
	 *
	 * Sets: REQUEST_METHOD, PATH_INFO, QUERY_STRING, CONTENT_TYPE,
	 * CONTENT_LENGTH, SERVER_PROTOCOL, HTTP_* headers, etc.
	 *
	 * @param req HTTP request
	 * @param scriptPath Path to CGI script
	 * @return std::vector<std::string> Array of "KEY=VALUE" strings for execve
	 */
	static std::vector<std::string> buildEnvironment(const HttpRequest& req,
													 const std::string& scriptPath);

	/**
	 * @brief Parse CGI output (headers + body)
	 *
	 * CGI scripts output:
	 *   Status: 200 OK\r\n
	 *   Content-Type: text/html\r\n
	 *   \r\n
	 *   <html>body</html>
	 *
	 * @param output Raw output from CGI script
	 * @return HttpResponse Parsed response
	 */
	static HttpResponse parseCgiOutput(const std::string& output);

	/**
	 * @brief Generate error response (500, 502, 504)
	 *
	 * @param statusCode HTTP error code
	 * @param message Error message
	 * @return HttpResponse Error response
	 */
	static HttpResponse generateErrorResponse(int statusCode, const std::string& message);

	/**
	 * @brief Convert string to integer (C++98 compatible)
	 */
	static int stringToInt(const std::string& str);

	/**
	 * @brief Convert integer to string (C++98 compatible)
	 */
	static std::string intToString(int n);

	/**
	 * @brief Trim whitespace from string
	 */
	static std::string trim(const std::string& str);

	/**
	 * @brief Check if file exists and is executable
	 */
	static bool isExecutable(const std::string& path);
};

#endif
