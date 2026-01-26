#include "router/Router.hpp"        // HttpResponse
#include "router/PathUtils.hpp"     // joinPath()

#include <dirent.h>                // opendir(), readdir(), closedir(), DIR, dirent
#include <sys/stat.h>              // stat(), struct stat, S_ISDIR, S_ISREG
#include <sstream>                 // std::ostringstream
#include <ctime>                   // std::localtime, std::strftime, time_t
// ----------------------------------------------------------------------------
// INTERNAL HELPERS (file-private)
// ----------------------------------------------------------------------------
// All helper functions are marked static so they are only visible inside this
// translation unit (AutoIndex.cpp). Only buildAutoIndexResponse() is public.
// ----------------------------------------------------------------------------

// Escape special HTML characters to avoid breaking HTML and to prevent injection.
// Example filename: a<b>.txt must display safely instead of creating a <b> tag.
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

// Ensure directory-like URLs end with '/'.
// This is mostly for display consistency ("Index of /dir/").
// Note: the real correctness comes from the Router redirect for "/dir" -> "/dir/".
static std::string ensureTrailingSlash(const std::string& p)
{
	if (!p.empty() && p[p.size() - 1] == '/')
		return (p);
	return (p + "/");
}

// Formats a time_t into something readable.
// Example: "2026-01-23 15:10:33"
static std::string formatTime(time_t t)
{
	char buf[64];
	std::tm* tmv = std::localtime(&t);
	if (!tmv)
		return ("");
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmv);
	return (std::string(buf));
}

// Call stat() on a filesystem path.
// Returns true if we can stat() the path and fill st.
static bool tryStat(const std::string& fullPath, struct stat& st)
{
	return (stat(fullPath.c_str(), &st) == 0);
}

// Build the HTML header: doctype, head, title, CSS, and table header.
// baseUrl should be the requested URL path for display, typically "/something/".
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

	return (html.str());
}

// Builds the bottom part of the HTML page.
static std::string buildAutoIndexFooterHtml()
{
	return ("  </tbody>\n</table>\n</body>\n</html>\n");
}

// Builds one row of the directory listing table.
static std::string buildAutoIndexRowHtml(const std::string& name, bool isDirectory,
	bool hasStat, const struct stat& st)
{
	// Directory links should end with '/', so browsers treat them as directories.
	// This makes navigation consistent (ex: clicking "files/" goes to ".../files/").
	std::string href = name;
	if (isDirectory)
		href += "/";

	std::ostringstream row;
	row << "<tr>";

	// Column 1: Name as a hyperlink (relative link).
	row << "<td><a href=\""
		<< htmlEscape(href) << "\">"
		<< htmlEscape(name)
		<< (isDirectory ? "/" : "")
		<< "</a></td>";

	// Column 2: Last modified time (if available).
	if (hasStat)
		row << "<td>" << htmlEscape(formatTime(st.st_mtime)) << "</td>";
	else
		row << "<td></td>";

	// Column 3: Size (only meaningful for regular files).
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
// outHtml: output string where final HTML is stored
static bool generateAutoIndexHtml(const std::string& fsDirPath, const std::string& urlPath,
									std::string& outHtml)
{
	/*If opendir fails:
	you cannot list directory
	maybe no permission
	maybe not a directory
	So return false and later you respond with 403.*/
	DIR* dir = opendir(fsDirPath.c_str());
	if (!dir)
		return (false);

	// Ensure base url ends with '/', looks nicer and is semantically correct
	std::string baseUrl = ensureTrailingSlash(urlPath);

	//use ostringstream because it’s easier to append many strings efficiently.
	std::ostringstream html;
	html << buildAutoIndexHeaderHtml(baseUrl);

	// Add a manual parent directory entry when not listing the URL root.
	// This is a user-friendly link to go up 1 level.
	if (baseUrl != "/")
	{
		html << "<tr><td><a href=\"../\">../</a></td><td></td><td></td></tr>\n";
	}


	// Iterate over all directory entries.
	// readdir() returns "." and ".." too; we skip them to avoid duplicates.
	for (struct dirent* ent = readdir(dir); ent != NULL; ent = readdir(dir))
	{
		std::string name = ent->d_name;

		// Skip current and parent pseudo-entries (we already add "../" manually).
		if (name == "." || name == "..")
			continue;

		// Compute full path on disk for metadata lookup:
		//   fsDirPath + "/" + name
		std::string fullPath = joinPath(fsDirPath, name);

		// Get metadata (type, size, modified time).
		struct stat st;
		bool hasStat = tryStat(fullPath, st);

		// Determine if it's a directory (only valid if stat succeeded).
		bool isDirectory = (hasStat && S_ISDIR(st.st_mode));

		// Append one row to the table.
		html << buildAutoIndexRowHtml(name, isDirectory, hasStat, st);
	}
	// Always close the directory handle to avoid leaking file descriptors.
	closedir(dir);

	html << buildAutoIndexFooterHtml();
	outHtml = html.str();
	return (true);
}

// Wraps the HTML into your HttpResponse type.
HttpResponse buildAutoIndexResponse(const std::string& fsDirPath, const std::string& urlPath)
{
	std::string body;

	// If we cannot open/read the directory => treat as forbidden
	if (!generateAutoIndexHtml(fsDirPath, urlPath, body))
		return (HttpResponse(403, "Forbidden"));

	HttpResponse resp(200, "OK");
	resp.headers["Content-Type"] = "text/html; charset=utf-8";
	resp.body = body;
	return (resp);
}

