#!/usr/bin/env perl

use strict;
use warnings;
use CGI;

my $q = CGI->new;
my $name = $q->param('name') || 'World';


print <<"HTML";
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Perl CGI Greeting</title>
    <style>
HTML

print <<'CSS';
        body {
            background: linear-gradient(to right, #dbefff, #a3d7dd);
            color: #1b2a2f;
            font-family: 'Segoe UI', sans-serif;
            text-align: center;
            padding-top: 100px;
        }
        h1 {
            font-size: 3em;
            animation: fadeIn 2s ease-in-out;
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-20px); }
            to { opacity: 1; transform: translateY(0); }
        }
        .box {
            background: rgba(78, 160, 177, 0.8);
            border-radius: 15px;
            padding: 30px;
            display: inline-block;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }
CSS

print <<"HTML";
    </style>
</head>
<body>
    <div class="box">
        <h1>Hello, $name!</h1>
        <p>This is a Perl CGI test 🐪</p>
    </div>
</body>
</html>
HTML

