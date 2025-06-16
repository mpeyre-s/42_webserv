#include "../includes/Server.hpp"

Location::Location() {}

Location::~Location() {}

Server::Server() {}

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

int Server::getMaxBodySize() const {
	return client_max_body_size;
}

std::map<int, std::string> Server::getErrorPages() const {
	return error_pages;
}

/**========================================================================
 *                           Setters
 *========================================================================**/

void Server::setServerName(const std::string& server) {
	server_name = server;
}
void Server::setHost(std::string& hostname){
	host = hostname;
}
void Server::setPort(int& porttmp){
	port = porttmp;
}
void Server::setRoot(std::string& rootname){
	root = rootname;
}
void Server::setIndex(std::string& indexname){
	index = indexname;
}

void Server::setLocation(const std::string path, Location& newLocation){
	locations[path] = newLocation;
}

void Server::addErrorPage(int err, std::string mess)
{
	error_pages[err] = mess;
}
void Server::addAllowedMethod(std::string method){
	allowed_methods.push_back(method);
}

void Server::setClientMaxBody(int clientMaxBody){
	client_max_body_size =  clientMaxBody;
}

