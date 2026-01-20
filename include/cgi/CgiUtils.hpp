#ifndef CGIUTILS_HPP
#define CGIUTILS_HPP

#include "../http/Response.hpp"
#include <string>

/**
 * @brief CGI Utilities - Helper functions for CGI handler
 *
 * Provides utility functions for string conversion, trimming,
 * file checks, and error response generation.
 */
namespace CgiUtils {
    /**
     * @brief Convert string to integer (C++98 compatible)
     */
    int stringToInt(const std::string& str);

    /**
     * @brief Convert integer to string (C++98 compatible)
     */
    std::string intToString(int n);

    /**
     * @brief Trim whitespace from string
     */
    std::string trim(const std::string& str);

    /**
     * @brief Check if file exists and is executable
     */
    bool isExecutable(const std::string& path);

    /**
     * @brief Generate error response (500, 502, 504)
     *
     * Creates a nicely formatted HTML error page for CGI errors.
     *
     * @param statusCode HTTP error code
     * @param message Error message
     * @return HttpResponse Error response
     */
    HttpResponse generateErrorResponse(int statusCode, const std::string& message);
}

#endif