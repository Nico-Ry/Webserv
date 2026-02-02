#ifndef UTILS_HPP
#define UTILS_HPP

#include "configParser/ConfigParser.hpp"
#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "router/Router.hpp"
#include <iomanip>

class Config;

//---------------------------------------------------------------------------//
//								ENTIRE CONFIG
//---------------------------------------------------------------------------//

void	printAllOutput(const Config& data);


//---------------------------------------------------------------------------//
//							 CONFIG SERVER BLOCKS
//---------------------------------------------------------------------------//

void	printServerPorts(const Config& data);
void	printServerRoot(const Config& data);
void	printServerIndex(const Config& data);
void	printServerErrorPages(const Config& data);
void	printServerAutoIndex(const Config& data);
void	printServerMaxSize(const Config& data);
void	printServerUpload(const Config& data);


//---------------------------------------------------------------------------//
//					   	   CONFIG LOCATION BLOCKS
//---------------------------------------------------------------------------//

void	printLocationRoot(const ServerBlock& s);
void	printLocationIndex(const ServerBlock& s);
void	printLocationErrorPages(const ServerBlock& s);
void	printLocationAutoIndex(const ServerBlock& s);
void	printLocationMaxSize(const ServerBlock& s);
void	printLocationMethods(const ServerBlock& data);
void	printLocationRedirect(const ServerBlock& data);
void	printLocationUpload(const ServerBlock& data);
void	printCgiInfo(const ServerBlock& s);


//---------------------------------------------------------------------------//
//						    HTTP REQUEST PARSER
//---------------------------------------------------------------------------//

void	printHttpMethod(const HttpMethod& method);
void	printHttpTarget(const HttpRequest& req);
void	printHttpVersion(const HttpRequest& req);
void	printHttpHeaders(const HttpRequest& req);
void	printHttpBody(const HttpRequest& req);
void	printHttpRequest(const HttpRequest& req);


//---------------------------------------------------------------------------//
//								ROUTER
//---------------------------------------------------------------------------//

void	printParentPaths(const DescendingStrSet& paths);
void	printRouterUri(const HttpRequest& r);

//---------------------------------------------------------------------------//
//							HTTP GENERATOR
//---------------------------------------------------------------------------//
std::string		generateErrorHtml(const int& statusCode, const std::string& statusMsg);
void			printNonSuccess(const HttpResponse& resp);
void			printSuccess(const HttpResponse& resp);


//---------------------------------------------------------------------------//
//							  LOG WARNINGS
//---------------------------------------------------------------------------//

void			logWarning(const std::string& msg);
void			logCustomErrorPage_Warning(const std::string& msg, const std::string& errorPagePath);
void			logCustomErrorPage_Error(const std::string& msg, const std::string& errorPagePath);

#endif
