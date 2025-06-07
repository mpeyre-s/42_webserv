#include "../includes/Server.hpp"

Location::Location() : alias(false), client_body_buffer_size(0) {}

Location::~Location() {}

Server::Server() {
    server_name = "webserv";
    host = "127.0.0.1";
    port = 8080;
    root = "/var/www/html";
    index = "index.html";
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");

    Location root_location;
    root_location.path = "/";
	root_location.allowed_methods.push_back("GET");
	root_location.allowed_methods.push_back("POST");
	root_location.allowed_methods.push_back("DELETE");
    root_location.root = "/var/www/html";
    root_location.index = "index.html";
    root_location.alias = false;
    root_location.client_body_buffer_size = 1024 * 1024; // 1MB
    locations["/"] = root_location;

    // Server images
    Location images_location;
    images_location.path = "/images";
    images_location.allowed_methods.push_back("GET");
    images_location.root = "/var/www/images";
    images_location.index = "index.html";
    images_location.alias = true;
    images_location.client_body_buffer_size = 5 * 1024 * 1024; // 5MB
    locations["/images"] = images_location;

    // CGI
    Location api_location;
    api_location.path = "/api";
    api_location.allowed_methods.push_back("GET");
	api_location.allowed_methods.push_back("POST");
    api_location.root = "/var/www/api";
    api_location.index = "api.php";
    api_location.alias = false;
    api_location.client_body_buffer_size = 2 * 1024 * 1024; // 2MB
    api_location.cgi_pass = "/usr/bin/php-cgi";
    locations["/api"] = api_location;
}

Server::~Server() {}

std::string Server::getServerName() const {
	return server_name;
}

std::string Server::getHost() const {
	return host;
}

int Server::getPort() const {
	return port;
}

std::string Server::getRoot() const {
	return root;
}

std::string Server::getIndex() const {
	return index;
}

const std::vector<std::string>& Server::getAllowedMethods() const {
	return allowed_methods;
}

const std::map<std::string, Location>& Server::getLocations() const {
	return locations;
}
