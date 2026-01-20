#include "../../include/cgi/CgiParser.hpp"
#include "../../include/cgi/CgiUtils.hpp"
#include "../../include/http/Status.hpp"
#include <sstream>

HttpResponse CgiParser::parseCgiOutput(const std::string& output) {
    HttpResponse resp;

    // Find empty line separating headers from body
    size_t header_end = output.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        header_end = output.find("\n\n");
    }

    if (header_end == std::string::npos) {
        // No headers, entire output is body
        resp.statusCode = 200;
        resp.reason = "OK";
        resp.body = output;
        resp.headers["Content-Type"] = "text/html";
        return resp;
    }

    // Parse headers
    std::string header_section = output.substr(0, header_end);
    std::string body_section = output.substr(header_end + (output[header_end] == '\r' ? 4 : 2));

    // Default values
    resp.statusCode = 200;
    resp.reason = "OK";

    // Parse each header line
    std::istringstream header_stream(header_section);
    std::string line;

    while (std::getline(header_stream, line)) {
        // Remove \r if present
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line = line.substr(0, line.size() - 1);
        }

        if (line.empty()) continue;

        // Find colon separator
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;

        std::string key = CgiUtils::trim(line.substr(0, colon_pos));
        std::string value = CgiUtils::trim(line.substr(colon_pos + 1));

        // Special handling for "Status" header
        if (key == "Status") {
            // Parse "Status: 200 OK" or "Status: 404"
            size_t space_pos = value.find(' ');
            if (space_pos != std::string::npos) {
                resp.statusCode = CgiUtils::stringToInt(value.substr(0, space_pos));
                resp.reason = CgiUtils::trim(value.substr(space_pos + 1));
            } else {
                resp.statusCode = CgiUtils::stringToInt(value);
                resp.reason = reasonPhrase(resp.statusCode);
            }
        } else {
            // Regular header
            resp.headers[key] = value;
        }
    }

    resp.body = body_section;

    // Set default Content-Type if not provided
    if (resp.headers.find("Content-Type") == resp.headers.end()) {
        resp.headers["Content-Type"] = "text/html";
    }

    return resp;
}