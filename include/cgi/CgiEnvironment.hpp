#ifndef CGIENVIRONMENT_HPP
#define CGIENVIRONMENT_HPP

#include "../http/Request.hpp"
#include <string>
#include <vector>

/**
 * @brief CGI Environment Builder - Constructs CGI environment variables
 *
 * Responsible for building the complete set of CGI/1.1 environment variables
 * from an HTTP request.
 */
namespace CgiEnvironment {
    /**
     * @brief Build CGI environment variables from HTTP request
     *
     * Creates all standard CGI environment variables:
     * - REQUEST_METHOD, PATH_INFO, QUERY_STRING
     * - CONTENT_TYPE, CONTENT_LENGTH
     * - SERVER_PROTOCOL, SERVER_NAME, SERVER_PORT
     * - HTTP_* for all request headers
     * - GATEWAY_INTERFACE, SERVER_SOFTWARE
     * - REDIRECT_STATUS
     *
     * @param req HTTP request
     * @param scriptPath Path to CGI script
     * @return std::vector<std::string> Array of "KEY=VALUE" strings for execve
     */
    std::vector<std::string> buildEnvironment(const HttpRequest& req,
                                              const std::string& scriptPath);
}

#endif