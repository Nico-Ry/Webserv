# WWW Directory - Static Files

This directory contains all HTML/CSS files served by the webserver.

## Structure

```
www/
├── css/
│   └── style.css       # Common CSS shared by all pages
└── pages/
    ├── index.html      # Home page (route: /)
    ├── test.html       # Test page (route: /test)
    ├── upload.html     # Upload page (route: /upload)
    └── 404.html        # Error page (404 Not Found)
```

## Current Status

**Temporary Implementation**: The HTML files here are currently NOT being served by the server. Instead, Server.cpp still has hardcoded HTML generation functions (`generateHomePage()`, `generateTestPage()`, etc.).

**Future Implementation**: When the FileHandler is implemented, it should:
1. Read HTML files from this directory
2. Serve them based on the request path
3. Handle the CSS file properly via MIME types

## Integration Steps (TODO)

1. Implement FileHandler class to read files from disk
2. Update Router to dispatch file serving requests to FileHandler
3. Remove hardcoded HTML functions from Server.cpp:
   - `getCommonCSS()`
   - `generateHomePage()`
   - `generateTestPage()`
   - `generateUploadPage()`
   - `generate404Page()`
4. Configure the server's root directory to point to this `www/` folder

## Notes

- CSS is linked with `<link rel="stylesheet" href="/css/style.css">` in each HTML file
- All pages share the same navigation bar
- The test page has interactive JavaScript for testing HTTP methods
- The upload page is a placeholder (UploadHandler not yet implemented)