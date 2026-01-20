#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "http/Response.hpp"
#include "router/Router.hpp"

Router::Router() {}

Router::~Router() {}

HttpResponse Router::handleHttpRequest(const HttpRequest& req) {
	(void)req;  // Unused for now - will be used when Router is implemented

	HttpResponse	resp(200, "OK");
	resp.headers["Content-Type"] = "text/html";
	resp.body = "<html><body><h1>Router stub</h1><p>Router implementation coming soon!</p></body></html>";

	// Build response logic from here
	// This will be implemented by your teammates

	return resp;
}