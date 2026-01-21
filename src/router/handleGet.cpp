#include "router/Router.hpp"

RouteResult	Router::handleGet(const std::string& uri) {
	// GET LOGIC GOES HERE


	(void)uri;
	RouteResult	success(200, "OK");
	return success;
}