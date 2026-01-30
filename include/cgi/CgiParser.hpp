#ifndef CGIPARSER_HPP
#define CGIPARSER_HPP

#include "../http/HttpResponse.hpp"
#include <string>

/**
 * @brief CGI Output Parser - Parses CGI script output
 *
 * Parses the output from a CGI script, which consists of:
 * - Headers (Status, Content-Type, etc.)
 * - Empty line
 * - Body
 */
namespace CgiParser {
	/**
	 * @brief Parse CGI output (headers + body)
	 *
	 * CGI scripts output:
	 *   Status: 200 OK\r\n
	 *   Content-Type: text/html\r\n
	 *   \r\n
	 *   <html>body</html>
	 *
	 * Parses the headers and body, builds an HttpResponse.
	 * Special handling for "Status" header.
	 * Default Content-Type is text/html if not provided.
	 *
	 * @param output Raw output from CGI script
	 * @return HttpResponse Parsed response
	 */
	HttpResponse parseCgiOutput(const std::string& output);
}

#endif
