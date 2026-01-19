#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "http/Response.hpp"
#include "router/Router.hpp"

Router::Router() {}

Router::~Router() {}

HttpResponse Router::handleHttpRequest(const HttpRequest& req) {
	HttpResponse	resp;

	// Build response logic from here

	return resp;
}