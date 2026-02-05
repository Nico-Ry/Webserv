#!/usr/bin/env python3
"""
Simple CGI script that displays the current time
Also shows request information
"""

import os
from datetime import datetime

print("Content-Type: text/html; charset=UTF-8")
print("Content-Language: en")
print()

now = datetime.now()
request_method = os.environ.get('REQUEST_METHOD', 'UNKNOWN')
query_string = os.environ.get('QUERY_STRING', '')
path_info = os.environ.get('PATH_INFO', '')

print(f"""<!DOCTYPE html>
<html>
<head>
	<title>CGI Time</title>
	<style>
		body {{
			font-family: -apple-system, sans-serif;
			max-width: 700px;
			margin: 50px auto;
			padding: 20px;
			background: linear-gradient(135deg, #1e3a8a 0%, #3b82f6 100%);
			color: white;
			text-align: center;
		}}
		.time-display {{
			font-size: 72px;
			font-weight: 700;
			margin: 40px 0;
			text-shadow: 0 4px 8px rgba(0,0,0,0.3);
		}}
		.date-display {{
			font-size: 24px;
			margin-bottom: 40px;
			opacity: 0.9;
		}}
		.info-box {{
			background: rgba(255,255,255,0.1);
			backdrop-filter: blur(10px);
			border-radius: 12px;
			padding: 20px;
			margin-top: 30px;
			text-align: left;
		}}
		.info-row {{
			display: flex;
			justify-content: space-between;
			padding: 8px 0;
			border-bottom: 1px solid rgba(255,255,255,0.2);
		}}
		.info-row:last-child {{ border-bottom: none; }}
		.label {{ font-weight: 600; }}
		.value {{ font-family: 'Courier New', monospace; }}
	</style>
</head>
<body>
	<h1>🕐 Server Time</h1>
	<div class="time-display">{now.strftime('%H:%M:%S')}</div>
	<div class="date-display">{now.strftime('%A, %B %d, %Y')}</div>

	<div class="info-box">
		<h3>Request Information</h3>
		<div class="info-row">
			<span class="label">Method:</span>
			<span class="value">{request_method}</span>
		</div>
		<div class="info-row">
			<span class="label">Query String:</span>
			<span class="value">{query_string if query_string else "(empty)"}</span>
		</div>
		<div class="info-row">
			<span class="label">Path:</span>
			<span class="value">{path_info if path_info else "/"}</span>
		</div>
	</div>

	<p style="margin-top: 30px;"><a href="/" style="color: white;">← Back to home</a></p>
</body>
</html>""")
