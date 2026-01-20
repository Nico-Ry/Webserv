#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../configParser/Config.hpp"
#include <iostream>
#include <iomanip>
#include "colours.hpp"

class Config;
struct HttpRequest;
struct HttpResponse;

class Router {
	public:
		Router();
		~Router();

		const Config&	cfg;

		HttpResponse buildResponse(const HttpRequest& req, const int& clientPort);

		bool	isValidRequest(const HttpRequest& req, const int& clientPort);
		bool	validPort(const int& clientPort);
};

#endif