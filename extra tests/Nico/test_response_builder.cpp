#include <iostream>
#include "http/ResponseBuilder.hpp"
#include "http/Status.hpp"

/*
	Test Phase 3.9: build a raw HTTP response string.
*/

int	main()
{
	HttpResponse	resp;

	resp.statusCode = 200;
	resp.reason = reasonPhrase(200);
	resp.headers["Content-Type"] = "text/plain";
	resp.body = "Hello response builder";

	std::string	raw;

	raw = ResponseBuilder::build(resp, false);

	std::cout << "RAW RESPONSE:" << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << raw << std::endl;
	std::cout << "------------------------" << std::endl;

	return (0);
}
