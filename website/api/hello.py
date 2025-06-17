#!/usr/bin/env python3

import os
from urllib.parse import parse_qs

print("Content-Type: text/html\n")

query_string = os.environ.get("QUERY_STRING", "")
form = parse_qs(query_string)
name = form.get("name", ["World"])[0]

print(f"<html><body><h1>Hello, {name}!</h1></body></html>")
