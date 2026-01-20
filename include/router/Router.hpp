#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../http/Request.hpp"
#include "../http/Response.hpp"

class Router {
public:
	Router();
	~Router();

	// Static method to handle HTTP requests
	// This will be replaced with proper routing logic later
	static HttpResponse handleHttpRequest(const HttpRequest& req);
};

#endif