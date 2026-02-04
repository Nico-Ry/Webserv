#include <iostream>
#include "include/http/Request.hpp"
#include "include/http/Response.hpp"
#include "include/http/Status.hpp"

int main() {
	HttpRequest req;
	req.method = METHOD_GET;
	req.rawTarget = "/index.html";
	req.headers["Host"] = "localhost";

	HttpResponse resp(200, reasonPhrase(200));
	resp.headers["Content-Type"] = "text/plain";
	resp.body = "Hello test";

	std::cout << "Method: " << req.method << "\n";
	std::cout << "Target: " << req.rawTarget << "\n";
	std::cout << "Response: " << resp.statusCode
			  << " " << resp.reason << "\n";

	return 0;
}
