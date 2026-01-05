#include "http/Mime.hpp"

/*
	toLower(s)

	Simple lowercase helper to make extension matching case-insensitive.
*/
std::string	Mime::toLower(const std::string &s)
{
	std::string	out;
	size_t		i;

	out = s;
	i = 0;
	while (i < out.size())
	{
		if (out[i] >= 'A' && out[i] <= 'Z')
		{
			out[i] = static_cast<char>(out[i] - 'A' + 'a');
		}
		++i;
	}
	return (out);
}

/*
	getExtension(path)

	Returns the substring after the last '.' in the filename.
	If there is no '.', returns "".

	Example:
		"/a/b/index.html" -> "html"
		"photo.JPG" -> "JPG" (later lowercased by toLower)
*/
std::string	Mime::getExtension(const std::string &path)
{
	std::string::size_type	dotPos;
	std::string::size_type	slashPos;

	slashPos = path.find_last_of("/\\");
	dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos)
		return ("");
	/*
		Ensure the '.' is after the last slash.
		So "/a.b/file" does not treat ".b/file" as extension.
	*/
	if (slashPos != std::string::npos && dotPos < slashPos)
		return ("");
	if (dotPos + 1 >= path.size())
		return ("");
	return (path.substr(dotPos + 1));
}

/*
	fromPath(path)

	Returns a MIME type string.
	This is a minimal set, enough for webserv typical tests.
*/
std::string	Mime::fromPath(const std::string &path)
{
	std::string	ext;

	ext = toLower(getExtension(path));
	if (ext == "html" || ext == "htm")
		return ("text/html");
	if (ext == "css")
		return ("text/css");
	if (ext == "js")
		return ("application/javascript");
	if (ext == "json")
		return ("application/json");
	if (ext == "txt")
		return ("text/plain");
	if (ext == "xml")
		return ("application/xml");
	if (ext == "png")
		return ("image/png");
	if (ext == "jpg" || ext == "jpeg")
		return ("image/jpeg");
	if (ext == "gif")
		return ("image/gif");
	if (ext == "svg")
		return ("image/svg+xml");
	if (ext == "ico")
		return ("image/x-icon");
	if (ext == "pdf")
		return ("application/pdf");
	/*
		Default for unknown binary files.
	*/
	return ("application/octet-stream");
}
