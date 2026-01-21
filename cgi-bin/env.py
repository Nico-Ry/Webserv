#!/usr/bin/env python3
"""
CGI script that displays all environment variables
Useful for debugging CGI implementation
"""

import os
import cgi

print("Content-Type: text/html")
print()

print("""<!DOCTYPE html>
<html>
<head>
    <title>CGI Environment Variables</title>
    <style>
        body {
            font-family: -apple-system, sans-serif;
            max-width: 1000px;
            margin: 30px auto;
            padding: 20px;
            background: #f5f5f5;
        }
        h1 { color: #667eea; }
        table {
            width: 100%;
            border-collapse: collapse;
            background: white;
            box-shadow: 0 2px 8px rgba(0,0,0,0.1);
            border-radius: 8px;
            overflow: hidden;
        }
        th, td {
            text-align: left;
            padding: 12px 16px;
            border-bottom: 1px solid #e0e0e0;
        }
        th {
            background: #667eea;
            color: white;
            font-weight: 600;
        }
        tr:hover {
            background: #f8f9fa;
        }
        .code {
            font-family: 'Courier New', monospace;
            background: #f0f0f0;
            padding: 2px 6px;
            border-radius: 4px;
            font-size: 14px;
        }
    </style>
</head>
<body>
    <h1>🔍 CGI Environment Variables</h1>
    <p>These are all the environment variables passed to the CGI script:</p>
    <table>
        <thead>
            <tr>
                <th>Variable</th>
                <th>Value</th>
            </tr>
        </thead>
        <tbody>""")

# Sort environment variables for easier reading
for key in sorted(os.environ.keys()):
    value = os.environ[key]
    # Escape HTML special characters
    value = value.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')
    print(f'            <tr><td class="code">{key}</td><td>{value}</td></tr>')

print("""        </tbody>
    </table>
    <p style="margin-top: 30px;"><a href="/">← Back to home</a></p>
</body>
</html>""")