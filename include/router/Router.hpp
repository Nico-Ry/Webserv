#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include <iostream>
#include <iomanip>
#include "colours.hpp"

struct HttpRequest;
struct HttpResponse;

class Router {
	public:
		Router();
		~Router();

		static HttpResponse handleHttpRequest(const HttpRequest& req);
};

#endif