#pragma once

#include <string>
#include <iostream>
#include <map>

class Server {
private:
	class Location;

	std::string server_name;
	std::string host;
	int port;
	std::string root;
	std::string index;
	std::vector<std::string> allowed_methods;
	std::map<std::string, Location> locations;

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

		Location() : alias(false), client_body_buffer_size(0) {}
	};

public:
	Server();
	~Server();

	std::string getServerName() const;
};
