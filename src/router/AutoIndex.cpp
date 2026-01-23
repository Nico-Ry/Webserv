#include "router/Router.hpp"
#include "router/PathUtils.hpp"
#include <sys/stat.h>
#include <dirent.h>     // opendir, readdir, closedir
#include <ctime>

// Escapes HTML special chars so file names like "a<b>.txt" don't break the HTML.
static std::string htmlEscape(const std::string& s)
{
	std::string out;
	out.reserve(s.size());

	for (size_t i = 0; i < s.size(); ++i)
	{
		char c = s[i];
		if (c == '&') out += "&amp;";
		else if (c == '<') out += "&lt;";
		else if (c == '>') out += "&gt;";
		else if (c == '"') out += "&quot;";
		else out += c;
	}
	return out;
}

// Ensures URL paths that represent directories end with '/'.
// Example: "/images" -> "/images/"
static std::string ensureTrailingSlash(const std::string& p)
{
	if (!p.empty() && p[p.size() - 1] == '/')
		return p;
	return p + "/";
}

// Formats a time_t into something readable.
// Example: "2026-01-23 15:10:33"
static std::string formatTime(time_t t)
{
	char buf[64];
	std::tm* tmv = std::localtime(&t);
	if (!tmv)
		return "";
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmv);
	return std::string(buf);
}

// Returns true if we can stat() the path and fill st.
static bool tryStat(const std::string& fullPath, struct stat& st)
{
	return (stat(fullPath.c_str(), &st) == 0);
}

// Builds the top part of the HTML page (doctype, head, title, styling, header).
static std::string buildAutoIndexHeaderHtml(const std::string& baseUrl)
{
	std::ostringstream html;

	html << "<!doctype html>\n"
		 << "<html>\n"
		 << "<head>\n"
		 << "  <meta charset=\"utf-8\">\n"
		 << "  <title>Index of " << htmlEscape(baseUrl) << "</title>\n"
		 << "  <style>\n"
		 << "    body{font-family:system-ui,Segoe UI,Roboto,Arial,sans-serif;padding:24px;}\n"
		 << "    table{border-collapse:collapse;width:100%;}\n"
		 << "    th,td{padding:8px 10px;border-bottom:1px solid #ddd;text-align:left;}\n"
		 << "    a{text-decoration:none;}\n"
		 << "  </style>\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1>Index of " << htmlEscape(baseUrl) << "</h1>\n"
		 << "<table>\n"
		 << "  <thead>\n"
		 << "    <tr><th>Name</th><th>Last modified</th><th>Size</th></tr>\n"
		 << "  </thead>\n"
		 << "  <tbody>\n";

	return html.str();
}

// Builds the bottom part of the HTML page.
static std::string buildAutoIndexFooterHtml()
{
	return "  </tbody>\n</table>\n</body>\n</html>\n";
}

// Builds one row of the directory listing table.
static std::string buildAutoIndexRowHtml(
	const std::string& name,
	bool isDirectory,
	bool hasStat,
	const struct stat& st
)
{
	// If it's a directory, the link should end with '/', so browsers treat it as a folder.
	std::string href = name;
	if (isDirectory && name != "..")
		href += "/";

	std::ostringstream row;
	row << "<tr>";

	// Column 1: clickable name
	row << "<td><a href=\""
		<< htmlEscape(href) << "\">"
		<< htmlEscape(name)
		<< (isDirectory ? "/" : "")
		<< "</a></td>";

	// Column 2: last modified
	if (hasStat)
		row << "<td>" << htmlEscape(formatTime(st.st_mtime)) << "</td>";
	else
		row << "<td></td>";

	// Column 3: size (only for regular files)
	if (hasStat && S_ISREG(st.st_mode))
		row << "<td>" << st.st_size << "</td>";
	else
		row << "<td>-</td>";

	row << "</tr>\n";
	return row.str();
}

// Reads the directory and returns the full HTML for the listing.
// fsDirPath: filesystem directory path, example "/var/www/site/images"
// urlPath: requested URL, example "/images/"
static bool generateAutoIndexHtml(
	const std::string& fsDirPath,
	const std::string& urlPath,
	std::string& outHtml
)
{
	DIR* dir = opendir(fsDirPath.c_str());
	if (!dir)
		return false;

	// Ensure base url ends with '/', looks nicer and is semantically correct
	std::string baseUrl = ensureTrailingSlash(urlPath);

	std::ostringstream html;
	html << buildAutoIndexHeaderHtml(baseUrl);

	// Optional parent directory link (skip it for root "/")
	if (baseUrl != "/")
	{
		html << "<tr><td><a href=\"../\">../</a></td><td></td><td></td></tr>\n";
	}

	// Iterate directory entries
	for (struct dirent* ent = readdir(dir); ent != NULL; ent = readdir(dir))
	{
		std::string name = ent->d_name;

		// Skip current folder entry
		if (name == ".")
			continue;

		// joinPath must be YOUR existing function:
		// joinPath("/var/www", "file.txt") -> "/var/www/file.txt"
		std::string fullPath = joinPath(fsDirPath, name);

		struct stat st;
		bool hasStat = tryStat(fullPath, st);

		bool isDirectory = (hasStat && S_ISDIR(st.st_mode));

		html << buildAutoIndexRowHtml(name, isDirectory, hasStat, st);
	}

	closedir(dir);

	html << buildAutoIndexFooterHtml();
	outHtml = html.str();
	return true;
}

// Wraps the HTML into your HttpResponse type.
HttpResponse buildAutoIndexResponse(const std::string& fsDirPath, const std::string& urlPath)
{
	std::string body;

	// If we cannot open/read the directory => treat as forbidden
	if (!generateAutoIndexHtml(fsDirPath, urlPath, body))
		return HttpResponse(403, "Forbidden");

	HttpResponse resp(200, "OK");
	resp.headers["Content-Type"] = "text/html; charset=utf-8";
	resp.body = body;
	return resp;
}

