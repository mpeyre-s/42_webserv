#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "utils.hpp"

class Location {
public:
	std::string path; // dont
	std::vector<std::string> allowed_methods; // ok
	std::string root; // ok
	std::string index; // ok
	std::string upload_dir;
	bool auto_index; // ok
	int client_max_body_size; // ok
	std::vector<std::string> cgi_extensions; // ok
	std::string cgi_path; //ok
	int redirect_code; //ok
	std::string redirect_url; //ok
	std::map<int, std::string> error_pages; // ok

	Location(std::vector<std::string> locationBlock);
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
	bool auto_index;
	std::map<int, std::string> error_pages;

	std::map<std::string, Location*> locations;

public:
	Server(std::vector<std::string> confFile);
	~Server();

	std::string getServerName() const;
	std::string getHost() const;
	int getPort() const;
	std::string getRoot() const;
	std::string getIndex() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::map<std::string, Location*>& getLocations() const;
	int getMaxBodySize() const;
	bool getAutoIndex() const;
	std::map<int, std::string> getErrorPages() const;

	void setServerName(const std::string& server);
	void setHost(std::string& hostname);
	void setPort(int& porttmp);
	void setRoot(std::string& rootname);
	void setIndex(std::string& indexname);
	void addErrorPage(int err, std::string mess);
	void addAllowedMethod(std::string method);
	void setClientMaxBody(int clientMaxBody);
	void setLocation(const std::string, Location *location);
};
