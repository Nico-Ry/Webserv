#include <iostream>
#include "http/Mime.hpp"

/*
	Test MIME detection.
*/

int	main()
{
	std::cout << "index.html -> " << Mime::fromPath("index.html") << std::endl;
	std::cout << "style.CSS -> " << Mime::fromPath("style.CSS") << std::endl;
	std::cout << "app.js -> " << Mime::fromPath("app.js") << std::endl;
	std::cout << "photo.jpeg -> " << Mime::fromPath("photo.jpeg") << std::endl;
	std::cout << "file.unknown -> " << Mime::fromPath("file.unknown") << std::endl;
	std::cout << "/var/www/a/b/logo.PNG -> " << Mime::fromPath("/var/www/a/b/logo.PNG") << std::endl;

	return (0);
}
