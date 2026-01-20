# CGI Scripts Directory

This directory contains CGI scripts that can be executed by the Webserv server.

## Available Scripts

### hello.py
Simple greeting page demonstrating basic CGI functionality.
```
http://localhost:8080/cgi-bin/hello.py
```

### env.py
Displays all CGI environment variables. Useful for debugging.
```
http://localhost:8080/cgi-bin/env.py
```

### form.py
Interactive form that handles both GET and POST requests.
```
http://localhost:8080/cgi-bin/form.py
http://localhost:8080/cgi-bin/form.py?name=Alice&message=Hello
```

### time.py
Displays current server time and request information.
```
http://localhost:8080/cgi-bin/time.py
```

## Creating New CGI Scripts

### Requirements

1. **Shebang**: First line must specify the interpreter
   ```python
   #!/usr/bin/env python3
   ```

2. **Executable**: Script must have execute permissions
   ```bash
   chmod +x script.py
   ```

3. **HTTP Headers**: Must output at least Content-Type header
   ```python
   print("Content-Type: text/html")
   print()  # Empty line separates headers from body
   ```

### Minimal Example

```python
#!/usr/bin/env python3

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h1>Hello World!</h1>")
print("</body></html>")
```

## CGI Environment Variables

Your scripts have access to these environment variables:

- `REQUEST_METHOD` - HTTP method (GET, POST, DELETE)
- `QUERY_STRING` - Query parameters (?key=value)
- `PATH_INFO` - URL path
- `CONTENT_TYPE` - Request body type
- `CONTENT_LENGTH` - Request body size
- `HTTP_*` - All HTTP headers (e.g., HTTP_USER_AGENT)
- `SERVER_PROTOCOL` - HTTP version
- `SERVER_NAME` - Server hostname
- `SERVER_PORT` - Server port

## Reading Request Data

### GET - Query String

```python
import os
query = os.environ.get('QUERY_STRING', '')
# Parse: name=Alice&message=Hello
```

### POST - Request Body

```python
import sys
import os

content_length = int(os.environ.get('CONTENT_LENGTH', 0))
body = sys.stdin.read(content_length)
```

## Supported Languages

- Python (.py)
- PHP (.php)
- Perl (.pl)
- Shell (.sh)
- Ruby (.rb)

The server auto-detects the interpreter based on file extension.

## Debugging

If your script doesn't work:

1. Test it manually: `./cgi-bin/script.py`
2. Check permissions: `ls -l cgi-bin/script.py`
3. Verify shebang matches interpreter: `which python3`
4. View environment: http://localhost:8080/cgi-bin/env.py
5. Check server logs for execve errors

## Performance Notes

- Each CGI request spawns a new process (fork/exec)
- Scripts have a 30-second timeout
- For high-performance needs, consider FastCGI (future feature)