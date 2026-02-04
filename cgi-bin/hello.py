#!/usr/bin/env python3
import os
import urllib.parse

# 1. On récupère la chaîne brute (ex: "name=nico&x=1")
query_string = os.environ.get("QUERY_STRING", "")

# 2. On la parse pour en faire un dictionnaire
params = urllib.parse.parse_qs(query_string)

# On extrait les valeurs proprement (avec des valeurs par défaut)
name = params.get("name", ["Guest"])[0]
x_val = params.get("x", ["undefined"])[0]

print("Content-Type: text/html; charset=UTF-8")
print("Content-Language: en")
print("Status: 200 OK")
print()

print(f"""<!DOCTYPE html>
<html>
<head>
    <title>Python CGI - Hello</title>
    <style>
        body {{
            font-family: -apple-system, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }}
        .container {{
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }}
        h1 {{ font-size: 48px; margin-bottom: 20px; }}
        .data-box {{
            background: rgba(0,0,0,0.2);
            padding: 15px;
            border-left: 4px solid #fff;
            margin: 20px 0;
            font-family: monospace;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🐍 Hello, {name}!</h1>
        <p>Your ID (x) is: <strong>{x_val}</strong></p>
        
        <div class="data-box">
            Raw Query String: {query_string}
        </div>

        <p>Your Webserv server successfully passed these arguments to Python!</p>
        <p style="margin-top: 30px;"><a href="/" style="color: white;">← Back to home</a></p>
    </div>
</body>
</html>""")