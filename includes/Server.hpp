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
	std::string upload_dir;
	bool auto_index;
	int client_max_body_size;
	std::vector<std::string> cgi_extensions;
	std::string cgi_path;
	int redirect_code;
	std::string redirect_url;
	std::map<int, std::string> error_pages;

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
	int client_max_body_size;
	std::map<int, std::string> error_pages;
	std::map<std::string, Location> locations;

public:
	Server();
	~Server();

	std::string getServerName() const;
	std::string getHost() const;
	int getPort() const;
	std::string getRoot() const;
	std::string getIndex() const;
	int getClientMaxBodySize() const;
	const std::map<int, std::string> getErrorPages() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::map<std::string, Location>& getLocations() const;
};
