#ifndef MIME_HPP
#define MIME_HPP

#include <string>

/*
	MIME helper:

	Given a path (or filename), returns a Content-Type value.
	Example:
		"/var/www/index.html" -> "text/html"
		"image.png" -> "image/png"

	If unknown extension: "application/octet-stream"
*/
class Mime
{
public:
	static std::string	fromPath(const std::string &path);

	static std::string	toLower(const std::string &s);
private:
	static std::string	getExtension(const std::string &path);
};

#endif
