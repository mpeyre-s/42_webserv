#!/usr/bin/env php-cgi
<?php
// En-têtes CGI explicites (DOIVENT être les premières sorties)
header("Content-Type: text/html; charset=UTF-8");
// Ligne vide obligatoire après les en-têtes
echo "\r\n";

// Paramètre GET
$name = isset($_GET['name']) ? htmlspecialchars($_GET['name']) : "World";

// Contenu HTML
echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>PHP CGI Greeting</title>
<style>
body {
background: linear-gradient(to right, #dbefff, #a3d7dd);
color: #1b2a2f;
font-family: 'Segoe UI', sans-serif;
text-align: center;
padding-top: 100px;
margin: 0;
position: relative;
min-height: 100vh;
 }
.box {
background: rgba(78, 160, 177, 0.8);
border-radius: 15px;
padding: 30px;
display: inline-block;
box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
 }
h1 {
font-size: 3em;
animation: fadeIn 2s ease-in-out;
 }
@keyframes fadeIn {
 from { opacity: 0; transform: translateY(-20px); }
 to { opacity: 1; transform: translateY(0); }
 }
</style>
</head>
<body>
<div class="box">
<h1>Hello, $name!</h1>
<p>This is a PHP CGI test 🎈</p>
</div>
</body>
</html>
HTML;
?>
