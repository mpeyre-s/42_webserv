#!/usr/bin/env perl

use strict;
use warnings;
use CGI;

my $cgi = CGI->new;
my $message = $cgi->param("message") || "";

print "Content-Type: text/html; charset=UTF-8\n\n";

print <<'HTML_TOP';
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Message CGI</title>
    <style>
        body {
            margin: 0;
            padding: 0;
            background: linear-gradient(to right, #fdfbfb, #ebedee);
            font-family: 'Segoe UI', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            animation: fadeIn 1.5s ease-in-out;
        }
        .card {
            background: #ffffff;
            border-radius: 20px;
            padding: 2rem 3rem;
            box-shadow: 0 10px 25px rgba(0,0,0,0.1);
            text-align: center;
        }
        h2 {
            color: #333;
            font-size: 2em;
            margin-bottom: 1rem;
        }
        p {
            color: #555;
            font-size: 1.2em;
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
        }
    </style>
</head>
<body>
    <div class="card">
        <h2>Message reçu</h2>
        <p>
HTML_TOP

# Afficher le message en toute sécurité
print CGI::escapeHTML($message);

print <<'HTML_BOTTOM';
        </p>
    </div>
</body>
</html>
HTML_BOTTOM

