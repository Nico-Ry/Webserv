# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Webserv is an HTTP/1.1 server implementation in C++98, built from scratch without external libraries. The project recreates core features of nginx, including:
- Non-blocking I/O with poll-based multiplexing
- HTTP request parsing with chunked transfer encoding support
- Configuration file parsing (nginx-style syntax)
- Keep-alive connections
- Static file serving, CGI execution, and file uploads

## Build Commands

```bash
# Build the project
make

# Clean object files
make clean

# Full clean (objects + binary)
make fclean

# Rebuild from scratch
make re

# Run the server
./webserv <config_file>

# Example
./webserv config/default.conf
```

Note: The project requires C++98 standard compliance. Compiler flags: `-Wall -Wextra -Werror -std=c++98`

## Architecture: Big Picture

### Three-Layer Design

The codebase follows a modular architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────────┐
│                        LAYER 1: NETWORK                         │
│  (SocketManager, IOMultiplexer, Connection, Server)            │
│  - Creates and manages sockets (bind, listen, accept)          │
│  - Non-blocking I/O with poll()                                │
│  - Connection lifecycle management (recv/send buffers)         │
│  - Event loop coordination                                     │
└────────────────────┬────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────────┐
│                         LAYER 2: HTTP                           │
│  (RequestParser, ResponseBuilder, Request, Response, Mime)     │
│  - Incremental HTTP request parsing (state machine)           │
│  - Handles chunked encoding and Content-Length bodies         │
│  - HTTP response construction with automatic headers          │
│  - MIME type detection                                        │
│  - Keep-alive connection management                           │
└────────────────────┬────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────────┐
│                    LAYER 3: REQUEST HANDLING                    │
│  (Router, Config, ServerBlock, LocationBlock)                  │
│  - Configuration parsing and validation                       │
│  - URL routing and method validation                          │
│  - Handler dispatch (file serving, CGI, uploads, errors)      │
│  - Multi-server virtual hosting                               │
└─────────────────────────────────────────────────────────────────┘
```

### Key Data Flow

1. **Server::run()** (network/Server.cpp): Main event loop using poll()
   - Accepts new connections via SocketManager
   - Reads data into Connection recv_buffer
   - Calls processRequest() when data arrives

2. **Server::processRequest()** (network/Server.cpp):
   - Feeds recv_buffer to HttpRequestParser (one parser per client for keep-alive)
   - Parser builds HttpRequest incrementally (may need multiple recv() calls)
   - When parser.isDone() is true, calls handleHttpRequest()
   - Stores response in Connection send_buffer

3. **Request Handling Pipeline** (partially implemented):
   - Router matches URL path to LocationBlock from Config
   - Validates HTTP method is allowed
   - Dispatches to appropriate handler (file/CGI/upload/error)
   - Handler returns HttpResponse
   - ResponseBuilder serializes to HTTP format
   - Network layer sends response through socket

### Per-Connection State Management

Critical design pattern: Each client connection maintains its own parser instance in `std::map<int, HttpRequestParser*> parsers`. This enables:
- Keep-alive support (multiple sequential requests on same socket)
- Incremental parsing across multiple recv() calls
- Proper state isolation between clients

When parser.shouldCloseConnection() returns true (Connection: close header or HTTP/1.0), the connection and parser are cleaned up.

## Module Details

### Network Layer (COMPLETE)

Located in `include/network/` and `src/network/`

- **SocketManager**: Socket creation, binding, listening, accepting
- **IOMultiplexer**: Wraps poll() for monitoring multiple file descriptors
- **Connection**: Tracks per-client state (fd, recv_buffer, send_buffer)
- **Server**: Main event loop, client lifecycle management

Key implementation notes:
- All sockets are non-blocking (fcntl with O_NONBLOCK)
- Uses POLLIN/POLLOUT events for readable/writable detection
- Handles partial send() calls by keeping unsent data in send_buffer

### HTTP Layer (COMPLETE)

Located in `include/http/` and `src/http/`

Parser is split across multiple files:
- **RequestParser.Core1.cpp**: Main state machine (feed, reset, isDone, hasError)
- **RequestParser.StartLine3.cpp**: Parses "METHOD /path HTTP/version"
- **RequestParser.Headers4.cpp**: Header parsing with case-insensitive keys
- **RequestParser.Body5.cpp**: Content-Length and chunked transfer encoding
- **RequestParser.Connection6.cpp**: Keep-alive connection handling
- **RequestParser.Utils2.cpp**: Helper functions (trim, toLower, readLine)
- **ResponseBuilder7.cpp**: HTTP response serialization with automatic headers
- **Mime8.cpp**: MIME type detection from file extensions

Parser features:
- Incremental parsing (handles data arriving in chunks)
- HTTP/1.0 and HTTP/1.1 support
- Required Host header validation (HTTP/1.1)
- Chunked encoding with proper CRLF handling
- Body size limits and error detection (400, 413, 501, 505, etc.)

### Configuration Parser (COMPLETE)

Located in `include/configParser/` and `src/configParser/`

Parsing flow (see main.cpp header comment):
1. **Tokeniser** reads config file and produces token stream
2. **ConfigParser** consumes tokens and builds object hierarchy
3. **parseServerBlock.cpp** parses server{} directives
4. **parseLocationBlock.cpp** parses location{} directives nested inside servers

Data structures:
- **Config**: Top-level container with vector of ServerBlock
- **ServerBlock**: Stores port, root, index, autoIndex, errorPages, clientMaxBodySize, locations
- **LocationBlock**: Stores URI, methods, root, index, autoIndex, errorPages, redirects

Important: LocationBlock inherits defaults from parent ServerBlock. Location-specific settings override server-level settings at runtime.

Validation rules enforced:
- Each server must have port and root
- No duplicate ports across servers
- Various syntax validations during parsing

Example config syntax:
```nginx
server {
    listen 8080;
    root /var/www;
    index index.html;
    autoIndex on;
    clientMaxBodySize 10M;

    location / {
        methods GET POST DELETE;
        root /var/www/html;
    }

    location /uploads {
        methods POST;
        clientMaxBodySize 50M;
    }
}
```

### Router Layer (STUB IMPLEMENTATION)

Located in `include/router/Router.hpp` and `src/router/Router.cpp`

Current status: Skeleton only. Router::handleHttpRequest() returns empty HttpResponse.

Expected responsibilities:
- Match HttpRequest.path against LocationBlock URIs
- Validate HttpRequest.method against location's allowed methods
- Apply configuration inheritance (location overrides server defaults)
- Dispatch to appropriate handler based on path and method
- Return 405 Method Not Allowed for disallowed methods
- Return 404 Not Found for unmatched paths

### Missing Components

These handlers need to be implemented:

1. **File Handler**: Serve static files from filesystem
   - Open and read files based on root + URI
   - Generate directory listings (if autoIndex enabled)
   - Return 404/403/500 as appropriate
   - Set Content-Type from MIME detection

2. **CGI Handler**: Execute CGI scripts
   - Fork/exec external programs
   - Pass CGI environment variables (PATH_INFO, QUERY_STRING, REQUEST_METHOD, etc.)
   - Pipe request body to stdin, capture stdout
   - Timeout handling for long-running scripts

3. **Upload Handler**: Process file uploads
   - Parse multipart/form-data boundaries
   - Save uploaded files to disk
   - Enforce clientMaxBodySize limits
   - Return 201 Created or 413 Payload Too Large

4. **Error Handler**: Generate error pages
   - Use custom error pages from config if defined
   - Generate default HTML error pages otherwise
   - Handle cascading fallback (location errorPages -> server errorPages -> default)

## Important Design Decisions

### Why One Parser Per Connection?
Keep-alive connections send multiple requests sequentially on the same socket. Each request needs independent parsing state. Without per-connection parsers, pipelined requests would corrupt each other's state.

### Why State Machine Parser?
HTTP requests arrive in arbitrary chunks from recv(). A line-by-line parser blocks waiting for complete lines, but we need non-blocking operation. The state machine (PS_START_LINE → PS_HEADERS → PS_BODY → PS_DONE) processes whatever data is available and pauses when more is needed.

### Why Split Parser Into Multiple .cpp Files?
The parser is complex (~500+ lines). Splitting by responsibility (StartLine, Headers, Body, Connection) improves maintainability and allows parallel development.

### Configuration Inheritance Model
Config parsing only builds the structure. Runtime resolution of "which setting applies?" happens during request handling by checking LocationBlock first, then falling back to ServerBlock defaults.

## Development Workflow

When modifying network code:
- Remember all I/O is non-blocking; check for EAGAIN/EWOULDBLOCK
- Update both recv_buffer and send_buffer appropriately
- Ensure proper cleanup in removeClient()

When modifying HTTP parser:
- Test with chunked encoding, pipelined requests, and partial data
- Validate against both HTTP/1.0 and HTTP/1.1 requirements
- Use provided debug utilities in src/debug/httpParserUtils.cpp

When adding new handlers:
- Follow the pattern: take HttpRequest, return HttpResponse
- Use ResponseBuilder to serialize responses
- Check LocationBlock settings first, then ServerBlock defaults
- Set appropriate status codes and Content-Type headers

## Testing Strategy

The project currently lacks a formal test suite. When adding tests:
- Use config/error/*.conf files to test config validation
- Test incremental parsing by feeding data byte-by-byte
- Test keep-alive with multiple sequential requests
- Test chunked encoding with various chunk sizes
- Test all HTTP methods (GET, POST, DELETE)
- Test error conditions (400, 404, 405, 413, 500, 501, 505)

## Debug Utilities

Located in `src/debug/`:
- **httpParserUtils.cpp**: Print HttpRequest structures
- **configParserUtils.cpp**: Print Config/ServerBlock/LocationBlock trees

To enable debug output, uncomment the printAllOutput() call in main.cpp:89

## Current Branch Status

Branch: integration-1
Recent work: Integration of network and HTTP layers, router skeleton added

Known issues:
- Router.cpp/.hpp not fully implemented
- No file/CGI/upload/error handlers yet
- Server currently generates hardcoded HTML responses (see Server.cpp:140+)
- Multiple server support exists in config but Server.cpp only uses servers[0]

Next priorities:
1. Complete Router implementation (path matching, method validation)
2. Implement File Handler for static file serving
3. Connect handlers through Router
4. Add multi-server support (currently only first server in config is used)