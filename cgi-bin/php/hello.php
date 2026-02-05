#!/usr/bin/php
<?php
header("Content-Type: text/plain");

echo "Hello from PHP CGI!\n";
echo "PHP version: " . PHP_VERSION . "\n";
echo "Request method: " . $_SERVER['REQUEST_METHOD'] . "\n";
echo "Script name: " . $_SERVER['SCRIPT_NAME'] . "\n";
?>
