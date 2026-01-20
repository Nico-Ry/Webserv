#include "../../include/cgi/CgiEnvironment.hpp"
#include "../../include/cgi/CgiUtils.hpp"
#include <map>

std::vector<std::string> CgiEnvironment::buildEnvironment(const HttpRequest& req,
                                                          const std::string& scriptPath) {
    std::vector<std::string> env;

    // REQUEST_METHOD (mandatory)
    std::string method;
    if (req.method == METHOD_GET) method = "GET";
    else if (req.method == METHOD_POST) method = "POST";
    else if (req.method == METHOD_DELETE) method = "DELETE";
    else method = "UNKNOWN";
    env.push_back("REQUEST_METHOD=" + method);

    // QUERY_STRING (everything after '?' in URL)
    env.push_back("QUERY_STRING=" + req.query);

    // PATH_INFO (the path portion of URL)
    env.push_back("PATH_INFO=" + req.path);

    // SCRIPT_NAME
    env.push_back("SCRIPT_NAME=" + scriptPath);

    // SERVER_PROTOCOL
    env.push_back("SERVER_PROTOCOL=" + req.httpVersion);

    // CONTENT_TYPE
    std::map<std::string, std::string>::const_iterator ct_it = req.headers.find("content-type");
    if (ct_it != req.headers.end()) {
        env.push_back("CONTENT_TYPE=" + ct_it->second);
    }

    // CONTENT_LENGTH
    if (req.hasContentLength) {
        env.push_back("CONTENT_LENGTH=" + CgiUtils::intToString(req.contentLength));
    } else {
        env.push_back("CONTENT_LENGTH=" + CgiUtils::intToString(req.body.size()));
    }

    // HTTP_* headers (convert all request headers to CGI format)
    for (std::map<std::string, std::string>::const_iterator it = req.headers.begin();
         it != req.headers.end(); ++it) {

        std::string key = it->first;
        std::string value = it->second;

        // Convert header name to uppercase with HTTP_ prefix
        // Example: "user-agent" -> "HTTP_USER_AGENT"
        std::string env_key = "HTTP_";
        for (size_t i = 0; i < key.size(); i++) {
            char c = key[i];
            if (c == '-') {
                env_key += '_';
            } else if (c >= 'a' && c <= 'z') {
                env_key += (c - 'a' + 'A');  // Convert to uppercase
            } else {
                env_key += c;
            }
        }

        env.push_back(env_key + "=" + value);
    }

    // SERVER_SOFTWARE
    env.push_back("SERVER_SOFTWARE=Webserv/1.0");

    // GATEWAY_INTERFACE
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // SERVER_NAME and SERVER_PORT (from Host header)
    std::map<std::string, std::string>::const_iterator host_it = req.headers.find("host");
    if (host_it != req.headers.end()) {
        std::string host = host_it->second;
        size_t colon_pos = host.find(':');
        if (colon_pos != std::string::npos) {
            env.push_back("SERVER_NAME=" + host.substr(0, colon_pos));
            env.push_back("SERVER_PORT=" + host.substr(colon_pos + 1));
        } else {
            env.push_back("SERVER_NAME=" + host);
            env.push_back("SERVER_PORT=80");
        }
    }

    // REDIRECT_STATUS (required by some PHP configurations)
    env.push_back("REDIRECT_STATUS=200");

    return env;
}