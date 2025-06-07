#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>

class Location {
public:
	std::string path;
	std::vector<std::string> allowed_methods;
	std::string root;
	std::string index;
	bool alias;
	int client_body_buffer_size;
	std::string cgi_pass;
	std::map<std::string, Location> nested_locations;

	Location();
	~Location();
};

class Server {
private:
	std::string server_name;
	std::string host;
	int port;
	std::string root;
	std::string index;
	std::vector<std::string> allowed_methods;
	std::map<std::string, Location> locations;

public:
	Server();
	~Server();

	std::string getServerName() const;
	std::string getHost() const;
	int getPort() const;
	std::string getRoot() const;
	std::string getIndex() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::map<std::string, Location>& getLocations() const;
};
