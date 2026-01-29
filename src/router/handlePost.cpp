#include "router/Router.hpp"
#include "router/PathUtils.hpp"
#include <fstream>
#include <ctime>

// ---- small helpers  ----

static bool isSafeFilename(const std::string& name)
{
	if (name.empty())
		return (false);

	// Reject path separators and traversal patterns
	if (name.find('/') != std::string::npos)
		return (false);
	if (name.find('\\') != std::string::npos)
		return (false);
	if (name.find("..") != std::string::npos)
		return (false);

	// Optional: reject weird empty-dot names
	if (name == "." || name == "..")
		return (false);

	return (true);
}

static std::string lastPathSegmentOrEmpty(const std::string& urlPath)
{
	// urlPath is already req.path (no query)
	// Examples:
	//   "/upload/file.txt" -> "file.txt"
	//   "/upload/"         -> ""
	//   "/upload"          -> "upload" (not what we want if location is "/upload")
	//
	// We only treat it as “filename in URL” if there is something after the last '/'
	// AND the path does not end with '/'

	if (urlPath.empty())
		return ("");

	if (urlPath[urlPath.size() - 1] == '/')
		return ("");

	size_t pos = urlPath.rfind('/');
	if (pos == std::string::npos)
		return ("");

	return (urlPath.substr(pos + 1));
}

static std::string generateUploadName()
{
	static unsigned long counter = 0;
	++counter;

	//Unix epoch seconds
	std::time_t t = std::time(NULL);
	std::ostringstream oss;
	oss << "upload_file_" << (unsigned long)t << "_" << counter << ".bin";
	return (oss.str());
}

static std::string ensureTrailingSlash(const std::string& s)
{
	if (s.empty() || s[s.size() - 1] == '/')
		return (s);
	return (s + "/");
}

static HttpResponse	validateUploadDirOrFail(const LocationBlock* rules)
{
	// 3) Upload must be configured (for non-CGI POST)
	if (rules->uploadDir.empty())
		return (HttpResponse(403, "Forbidden"));

	// 4) Validate upload directory exists and is a directory
	if (!exists(rules->uploadDir) || !isDir(rules->uploadDir))
		return (HttpResponse(500, "Internal Server Error"));

	// 5) Need X to traverse + W to create files inside
	if (!canTraverseDir(rules->uploadDir) || access(rules->uploadDir.c_str(), W_OK) != 0)
		return (HttpResponse(403, "Forbidden"));

	return (HttpResponse(0, "")); // sentinel OK
}


static HttpResponse	writeBodyToFileOrFail(const std::string& fullPath,
											const HttpRequest& req)
{
	// 9) Write bytes (binary safe)
	//
	//	std::ios::binary => no byte transformations (raw bytes)
	//	std::ios::trunc  => overwrite existing file (truncate to 0 then write)
	std::ofstream ofs(fullPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!ofs.is_open())
		return (HttpResponse(500, "Internal Server Error"));

	// Write EXACTLY req.body.size() bytes (works even with '\0' bytes)
	ofs.write(req.body.data(), req.body.size());

	// good() becomes false if write failed
	if (!ofs.good())
		return (HttpResponse(500, "Internal Server Error"));

	ofs.close();

	/* Alternative: POSIX open/write (binary-safe by nature)
	//
	// int fd = open(fullPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	//  - O_WRONLY: write-only
	//  - O_CREAT : create if missing
	//  - O_TRUNC : overwrite if exists
	//  - 0644    : permissions if created (rw-r--r--)
	//
	// if (fd < 0) -> open failed (permissions, missing dirs, etc.)
	//
	// ssize_t written = write(fd, req.body.data(), req.body.size());
	//  - returns number of bytes written, or -1 on error
	//  - can be a partial write (robust code would loop)
	//
	// close(fd) always to avoid fd leaks
	*/

	return (HttpResponse(0, "")); // sentinel OK
}


/*@note It will upload anything (PNG/JPG/PDF/ZIP…), as long as the client
 sends the body as raw bytes (like  with --data-binary )
 printf 'hello\n' | curl -v -X POST --data-binary @- http://127.0.0.1:8080/upload/hello.txt),
because its writen in binary mode*/
HttpResponse Router::handlePost(const HttpRequest& req)
{
	// 1) Resolve path (useful for CGI decision & safety checks)
	std::string resolved = getResolvedPath(req.path, *rules);

	// Until CGI exists, avoid -Werror unused warning
	(void)resolved;

	// 2) CGI hook (Dibran will implement)
	// TODO(CGI): if this target should be executed as CGI, delegate here.
	// if (isCgiTarget(resolved, *rules))
	//     return (executeCgi(req, resolved, *rules));

	// 3-5) Validate upload dir
	// {} limits the lifetime (scope) of err
	{
		HttpResponse err = validateUploadDirOrFail(rules);
		if (err.statusCode != 0)
			return (err);
	}

	// 6) Decide filename:
	std::string filename = lastPathSegmentOrEmpty(req.path);

	// Edge case: POST to exactly the location URI
	if (req.path == rules->uri)
		filename = "";

	if (filename.empty())
		filename = generateUploadName();

	// 7) Safety check against traversal / injection
	if (!isSafeFilename(filename))
		return (HttpResponse(400, "Bad Request"));

	// 8) Full filesystem path
	std::string fullPath = joinPath(rules->uploadDir, filename);

	// Track whether it existed (optional; choose a response policy)
	bool existedBefore = exists(fullPath);

	// 9) Write bytes
	{
		HttpResponse err = writeBodyToFileOrFail(fullPath, req);
		if (err.statusCode != 0)
			return (err);
	}

	// 10) Build response
	/*
	Choose ONE policy ::
	Overwrite allowed → return 200 if existed, 201 if new (doing that right now)
	Overwrite forbidden → would return 409 Conflict or 403
	*/
	int	code;
	if (existedBefore)
		code = 200;
	else
		code = 201;

	std::string	msg;
	if (existedBefore)
		msg = "OK";
	else
		msg = "Created";

	// Avoid recomputing lastPathSegmentOrEmpty(req.path) twice
	bool urlHadFilename = (!lastPathSegmentOrEmpty(req.path).empty() && req.path != rules->uri);

	// “Location” header: where the uploaded resource can be accessed (URL-side)
	// If URL already had filename, it is req.path.
	// If generated, return rules->uri + "/" + filename.
	std::string locationUrl;
	if (!urlHadFilename)
		locationUrl = ensureTrailingSlash(rules->uri) + filename;
	else
		locationUrl = req.path;

	HttpResponse resp(code, msg, "uploaded\n");
	resp.headers["Content-Type: "] = "text/plain";
	resp.headers["Location: "] = locationUrl;
	return (resp);
}



/*tests:
printf 'hello\n' | curl -v -X POST --data-binary @- http://127.0.0.1:8080/upload/hello.txt

201 first time, 200 next time


printf 'abc123\n' | curl -v -X POST --data-binary @- http://127.0.0.1:8080/upload/

201

*/





/*
---Status Codes
201 Created → resource created (uploads)
204 No Content → success, no body
400 Bad Request → malformed request (bad headers, framing)
403 Forbidden → method understood but refused
404 Not Found → target missing
405 Method Not Allowed → POST not allowed for this location
413 Content Too Large → body too big
415 Unsupported Media Type → Content-Type unsupported
500 Internal Server Error → server/config error
502 bad CGI output (optional)
*/

/*
POST does not define behavior — server does
POST body is just bytes until you decide otherwise
Framing (Content-Length / chunked) is non-negotiable
CGI consumes POST via stdin
Upload is just “write body to disk”
Multipart is optional and complex
*/


/*POST decision flowchart (1 page)
A) Pre-routing checks (already in your routing flow)

These happen before deciding upload vs CGI.

Find matching location
	rules = getLocation(req.path)
	If no match → use server default location (or treat as 404 depending on your design)

Redirect?
	If rules.redirect set → return 301/302 with Location: ...

Method allowed?
	If POST not allowed in rules.methods
		→ 405 Method Not Allowed
		(+ Allow: ... header)

Max body size check
	If req.body.size() > rules.clientMaxBodySize (or server default)
	→ 413 Payload Too Large

Request parse errors
	If your parser marked the request invalid / bad framing
	→ 400 Bad Request

After these checks, POST is “permitted” and “safe to attempt”.

B) POST meaning selection (the core decision)

Now decide whether POST is CGI or upload.

Step 1 — Resolve filesystem path
	resolved = resolvePath(req.path, rules.root)
	If cannot resolve / invalid traversal
	→ 404 Not Found or 403 Forbidden (pick 1 policy and stick to it)

Step 2 — Is this a CGI request?
Condition (you will implement later with teammate):
	Either:
		resolved target has an extension configured as CGI (.py, .php, …)
		or location says “everything here is CGI”
		or path is inside cgi-bin mapping
	✅ If YES → go to CGI branch
	❌ If NO → go to Upload branch

C) CGI branch (POST handled by CGI)
1 Confirm executable/script exists
	If script missing → 404 Not Found
	If exists but not executable / forbidden → 403 Forbidden
2 Run CGI
	Provide:
		stdin = req.body
		env = REQUEST_METHOD=POST, CONTENT_LENGTH, CONTENT_TYPE, QUERY_STRING, PATH_INFO, etc.
	Read stdout, parse headers/status from CGI output
3 If CGI output invalid
	Bad CGI response format → 502 Bad Gateway (common choice) or 500
4 Return CGI-produced response
	Status + headers + body

Key point: CGI branch does not require upload directory.

D) Upload branch (POST handled by your server)
Step 1 — Is upload configured here?
	If rules.uploadEnabled == false (or upload dir missing)
		This means: POST was allowed, but there is no CGI and no upload destination
		Choose policy:
			Best: treat as config error → 500 Internal Server Error
			Acceptable: 403 Forbidden

Step 2 — Decide filename
Choose 1 of these policies:
Policy A (URL decides filename)
	If request is POST /upload/foo.txt
		filename = foo.txt
	If request ends with / or is /upload
		generate a name: upload_<timestamp>_<counter>.bin
Policy B (always generate)
	Always generate, ignore URL filename
	(this is simpler + safer but less “intuitive”)

Step 3 — Validate filename (mandatory)
Reject if:
	contains / or \
	contains ..
	empty
	too long (optional but nice)
If invalid → 400 Bad Request or 403 Forbidden

Step 4 — Write file
	fullPath = rules.uploadDir + "/" + filename
	Open with safe flags:
		create new or overwrite (choose 1)
	Write all bytes from req.body
	If failure (permission, disk error) → 500 Internal Server Error

Step 5 — Respond
	If created new file → 201 Created
	If overwritten existing → 200 OK or 204 No Content
	(pick 1 and keep consistent)
	Optional headers:
		Location: /upload/<filename>
		Content-Type: text/plain
	Body: short message is fine
*/

// it will be in rules.uploadDir
