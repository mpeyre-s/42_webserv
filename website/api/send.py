#!/usr/bin/env python3

import os
import sys
from urllib.parse import parse_qs

# Lire les données du POST
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
post_data = sys.stdin.read(content_length)
form = parse_qs(post_data)
message = form.get("message", ["(aucun message)"])[0]

print("Content-Type: text/html; charset=UTF-8")
print()

print(f"""\
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Message reçu</title>
    <style>
        body {{
            background: linear-gradient(145deg, #eef2f3, #dfe6e9);
            font-family: 'Segoe UI', sans-serif;
            color: #2c3e50;
            margin: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            height: 100vh;
        }}
        .card {{
            background: rgba(255, 255, 255, 0.85);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.1);
            padding: 40px;
            max-width: 500px;
            text-align: center;
            animation: fadeSlideIn 1.2s ease forwards;
            opacity: 0;
            transform: translateY(30px);
        }}
        h1 {{
            font-size: 2.2rem;
            margin-bottom: 20px;
            color: #34495e;
        }}
        p {{
            font-size: 1.2rem;
            color: #555;
        }}

        @keyframes fadeSlideIn {{
            to {{
                opacity: 1;
                transform: translateY(0);
            }}
        }}
    </style>
</head>
<body>
    <div class="card">
        <h1>📨 Nouveau message</h1>
        <p>{message}</p>
    </div>
</body>
</html>
""")




