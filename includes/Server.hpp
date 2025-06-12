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
	std::map<std::string, Location> nested_locations;

	Location();
	~Location();
};

class Server {
private:
	std::string server_name;  //ok
	std::string host; //ok voir erreurs
	int port; //ok voir erreurs
	std::string root; //ok
	std::string index; //ok
	std::vector<std::string> allowed_methods;
	int client_max_body_size; // ok
	std::map<int, std::string> error_pages; //ok
	std::map<std::string, Location> locations; //ok

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

	void setServerName(const std::string& server);
	void setHost(std::string& hostname);
	void setPort(int& porttmp);
	void setRoot(std::string& rootname);
	void setIndex(std::string& indexname);
	void addErrorPage(int err, std::string mess);
	void addAllowedMethod(std::string method);
	void setClientMaxBody(int clientMaxBody);
	void setLocation(const std::string, Location& location);
};
