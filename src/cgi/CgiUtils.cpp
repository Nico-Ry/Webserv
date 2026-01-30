#include "../../include/cgi/CgiUtils.hpp"
#include "../../include/http/Status.hpp"
#include <unistd.h>
#include <sstream>

int CgiUtils::stringToInt(const std::string& str)
{
	std::istringstream iss(str);
	int result;
	iss >> result;
	return (result);
}

std::string CgiUtils::intToString(int n)
{
	std::ostringstream oss;
	oss << n;
	return (oss.str());
}

std::string CgiUtils::trim(const std::string& str)
{
	size_t start = 0;
	size_t end = str.size();

	// Trim leading whitespace
	while (start < end && (str[start] == ' ' || str[start] == '\t' ||
						   str[start] == '\r' || str[start] == '\n'))
	{
		start++;
	}

	// Trim trailing whitespace
	while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t' ||
						   str[end - 1] == '\r' || str[end - 1] == '\n'))
	{
		end--;
	}

	return (str.substr(start, end - start));
}

bool CgiUtils::isExecutable(const std::string& path)
{
	return (access(path.c_str(), X_OK) == 0);
}

HttpResponse CgiUtils::generateErrorResponse(int statusCode, const std::string& message)
{
	HttpResponse resp(statusCode, reasonPhrase(statusCode));
	resp.headers["Content-Type: "] = "text/html";

	std::string html;
	html = "<!DOCTYPE html>\n<html>\n<head>\n";
	html += "    <title>CGI Error " + intToString(statusCode) + "</title>\n";
	html += "    <style>\n";
	html += "        body { font-family: sans-serif; max-width: 800px; margin: 50px auto; padding: 20px; }\n";
	html += "        h1 { color: #d32f2f; }\n";
	html += "        .error-box { background: #ffebee; border: 1px solid #ef5350; border-radius: 8px; padding: 20px; margin: 20px 0; }\n";
	html += "    </style>\n";
	html += "</head>\n<body>\n";
	html += "    <h1>CGI Error " + intToString(statusCode) + "</h1>\n";
	html += "    <div class=\"error-box\">\n";
	html += "        <strong>Error:</strong> " + message + "\n";
	html += "    </div>\n";
	html += "    <p><a href=\"/\">Return to home</a></p>\n";
	html += "</body>\n</html>";

	resp.body = html;
	return (resp);
}
