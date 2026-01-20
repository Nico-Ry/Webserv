#!/usr/bin/env python3
"""
CGI script that handles both GET and POST requests
Displays a form and processes form submissions
"""

import os
import sys
import cgi

# Read form data (works for both GET and POST)
form = cgi.FieldStorage()

# Get form values
name = form.getvalue('name', '')
message = form.getvalue('message', '')

print("Content-Type: text/html")
print()

print("""<!DOCTYPE html>
<html>
<head>
    <title>CGI Form Test</title>
    <style>
        body {
            font-family: -apple-system, sans-serif;
            max-width: 600px;
            margin: 50px auto;
            padding: 20px;
            background: #f5f5f5;
        }
        .container {
            background: white;
            border-radius: 12px;
            padding: 40px;
            box-shadow: 0 4px 16px rgba(0,0,0,0.1);
        }
        h1 { color: #667eea; }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 8px;
            font-weight: 600;
            color: #333;
        }
        input, textarea {
            width: 100%;
            padding: 12px;
            border: 1px solid #ddd;
            border-radius: 8px;
            font-size: 16px;
            font-family: inherit;
            box-sizing: border-box;
        }
        textarea {
            min-height: 120px;
            resize: vertical;
        }
        button {
            background: #667eea;
            color: white;
            border: none;
            padding: 12px 32px;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
        }
        button:hover {
            background: #5568d3;
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);
        }
        .result {
            background: #e7f5e7;
            border: 1px solid #4caf50;
            border-radius: 8px;
            padding: 20px;
            margin-bottom: 30px;
        }
        .method-badge {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 16px;
            font-size: 14px;
            font-weight: 600;
            margin-left: 10px;
        }
        .get { background: #10b981; color: white; }
        .post { background: #3b82f6; color: white; }
    </style>
</head>
<body>
    <div class="container">
        <h1>📝 CGI Form Test</h1>""")

# Display submission result if form was submitted
request_method = os.environ.get('REQUEST_METHOD', 'GET')

if name or message:
    print(f'''        <div class="result">
            <h3>✓ Form Submitted <span class="method-badge {request_method.lower()}">{request_method}</span></h3>
            <p><strong>Name:</strong> {name if name else "(empty)"}</p>
            <p><strong>Message:</strong> {message if message else "(empty)"}</p>
        </div>''')

print("""        <form method="POST" action="/cgi-bin/form.py">
            <div class="form-group">
                <label for="name">Your Name:</label>
                <input type="text" id="name" name="name" placeholder="Enter your name">
            </div>
            <div class="form-group">
                <label for="message">Message:</label>
                <textarea id="message" name="message" placeholder="Enter your message"></textarea>
            </div>
            <button type="submit">Submit (POST)</button>
        </form>

        <p style="margin-top: 30px; color: #666; font-size: 14px;">
            💡 This form uses POST. Try also with GET:
            <a href="/cgi-bin/form.py?name=John&message=Hello">?name=John&message=Hello</a>
        </p>

        <p style="margin-top: 20px;"><a href="/">← Back to home</a></p>
    </div>
</body>
</html>""")