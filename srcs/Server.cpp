#include "../includes/Server.hpp"

Location::Location() : client_max_body_size(0), redirect_code(0) {}

Location::~Location() {}

Server::Server() {
	server_name = "localhost";
	host = "0.0.0.0";
	port = 8080;
	root = "website/";
	index = "index.html";
	allowed_methods.push_back("GET");
	client_max_body_size = 1048576;

	error_pages[404] = "resources/not_found.html";
	error_pages[500] = "resources/internal_server_error.html";

	// Location /upload
	Location upload_loc;
	upload_loc.allowed_methods.push_back("POST");
	upload_loc.upload_dir = "website/media";
	upload_loc.client_max_body_size = 10;
	locations["/upload"] = upload_loc;

	// Location /files
	Location files_loc;
	files_loc.allowed_methods.push_back("GET");
	files_loc.allowed_methods.push_back("DELETE");
	files_loc.root = "website/media";
	files_loc.auto_index = true;
	locations["/files"] = files_loc;

	// Location /private
	Location private_loc;
	private_loc.redirect_code = 301;
	private_loc.redirect_url = "/";
	locations["/private"] = private_loc;

	// Location /cgi
	Location cgi_loc;
	cgi_loc.allowed_methods.push_back("GET");
	cgi_loc.allowed_methods.push_back("POST");
	cgi_loc.cgi_extensions.push_back(".py");
	cgi_loc.cgi_extensions.push_back(".pl");
	cgi_loc.cgi_extensions.push_back(".php");
	cgi_loc.cgi_path = "website/api";
	cgi_loc.auto_index = false;
	locations["/cgi"] = cgi_loc;
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

int Server::getClientMaxBodySize() const {
	return client_max_body_size;
}

const std::map<int, std::string> Server::getErrorPages() const {
	return error_pages;
}

std::string Server::getUploadDir() { // <== il faut le faire mais je sais pas comment
	return "website/media";
}
