#include "utils.hpp"


//---------------------------------------------------------------------------//
//						HTML & HTTP RESPONSE UTILS
//---------------------------------------------------------------------------//

void		printNonSuccess(const HttpResponse& resp) {
	if (resp.statusCode >= 400) {
	std::cout	<< std::left << BOLD_MAGENTA << std::setw(16) << "[HTTP Response]" << RES << "  ~   ["
				<< BOLD_RED << resp.statusCode << RES << "] ["
				<< BOLD_RED << resp.reason << RES << "]" << std::endl;
	}
	else {
	std::cout	<< std::left << BOLD_MAGENTA << std::setw(16) << "[HTTP Response]" << RES << "  ~   ["
				<< BOLD_BLUE << resp.statusCode << RES << "] ["
				<< BOLD_BLUE << resp.reason << RES << "]" << std::endl;
	}
}


void		printSuccess(const HttpResponse& resp) {
	std::cout	<< std::left << BOLD_MAGENTA << std::setw(16) << "[HTTP Response]" << RES << "  ~   ["
				<< BOLD_GREEN << resp.statusCode << RES << "] ["
				<< BOLD_GREEN << resp.reason << RES << "]" << std::endl;
}

void		logWarning(const std::string& msg) {
	std::cout	<< std::left << BOLD_ORANGE << std::setw(16) << "[Warning]"
				<< RES << "  ~  ["
				<< ORANGE << msg
				<< RES << "]" << std::endl;
}

void		logCustomErrorPage_Warning(const std::string& msg, const std::string& errorPagePath) {
	std::cout	<< std::left << BOLD_ORANGE << std::setw(16) << "[Warning]"
				<< RES << "  ~   " << msg << " -> '"
				<< RED << errorPagePath
				<< RES << "'" << std::endl;
}


void		logCustomErrorPage_Error(const std::string& msg, const std::string& errorPagePath) {
	std::cout	<< std::left << BOLD_RED << std::setw(16) << "[Error]"
				<< RES << "  ~   "
				<< RED << msg
				<< RES << " -> '"
				<< RED << errorPagePath
				<< RES << "'" << std::endl;
}



std::string	generateErrorHtml(const int& statusCode, const std::string& statusMsg)
{
	std::stringstream ss;

	ss <<
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <title>" << statusCode << " - " << statusMsg << "</title>\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\n"
	"    <link rel=\"stylesheet\" href=\"/css/common.css\">\n\n"
	"    <style>\n"
	"        .error-container {\n"
	"            text-align: center;\n"
	"            padding: var(--spacing-xl) 0;\n"
	"        }\n\n"
	"        .error-code {\n"
	"            font-size: 120px;\n"
	"            font-weight: 700;\n"
	"            color: #d35400;\n"
	"            line-height: 1;\n"
	"        }\n"
	"    </style>\n"
	"</head>\n"
	"<body>\n\n"
	"<nav class=\"navbar\">\n"
	"    <div class=\"navbar-content\">\n"
	"        <a href=\"/\" class=\"navbar-brand\">Webserv</a>\n"
	"        <ul class=\"navbar-links\">\n"
	"            <li><a href=\"/\">Home</a></li>\n"
	"            <li><a href=\"/test\">Test</a></li>\n"
	"            <li><a href=\"/upload\">Upload</a></li>\n"
	"        </ul>\n"
	"    </div>\n"
	"</nav>\n\n"
	"<div class=\"container\">\n"
	"    <div class=\"error-container\">\n"
	"        <div class=\"error-code\">" << statusCode << "</div>\n"
	"        <h1 style=\"font-size: 48px; margin: var(--spacing-md) 0;\">"
			<< statusMsg << "</h1>\n"
	"        <p style=\"color: #666; font-size: 18px; margin-bottom: var(--spacing-lg);\">\n"
	"            This error page was automatically generated.\n"
	"        </p>\n"
	"        <a href=\"/\" class=\"btn\">Go to Home</a>\n"
	"    </div>\n"
	"</div>\n\n"
	"</body>\n"
	"</html>";

	return ss.str();
}