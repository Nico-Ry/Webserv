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
