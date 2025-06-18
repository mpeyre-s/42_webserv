#!/usr/bin/env python3

import os
from urllib.parse import parse_qs

# Récupérer les paramètres GET
query_string = os.environ.get("QUERY_STRING", "")
form = parse_qs(query_string)
name = form.get("name", ["World"])[0]


print("Content-Type: text/html; charset=UTF-8")
print()  # Ligne vide obligatoire après les en-têtes

# HTML avec un peu de CSS et une animation
print(f"""\
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Greeting</title>
    <style>
        body {{
            background: linear-gradient(to right, #dbefff, #a3d7dd);
            color: #1b2a2f;
            font-family: 'Segoe UI', sans-serif;
            text-align: center;
            padding-top: 100px;
        }}
        h1 {{
            font-size: 3em;
            animation: fadeIn 2s ease-in-out;
        }}
        @keyframes fadeIn {{
            from {{ opacity: 0; transform: translateY(-20px); }}
            to {{ opacity: 1; transform: translateY(0); }}
        }}
        .box {{
            background: rgba(78, 160, 177, 0.8);
            border-radius: 15px;
            padding: 30px;
            display: inline-block;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }}
    </style>
</head>
<body>
    <div class="box">
        <h1>Hello, {name}!</h1>
        <p> 🎉 Welcome to the incredible CGI-GET tester 🎉</p>
    </div>
</body>
</html>
""")
