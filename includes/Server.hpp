#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "utils.hpp"

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
	Location(std::vector<std::string> locationBlock);
	~Location();

	std::string getPath() const;
	const std::vector<std::string>& getAllowedMethods() const;
	std::string getRoot() const;
	std::string getIndex() const;
	std::string getUploadDir() const;
	bool getAutoIndex() const;
	int getMaxBodySize() const;
	const std::vector<std::string>& getCgiExtensions() const;
	std::string getCgiPath() const;
	int getRedirectCode() const;
	std::string getRedirectUrl() const;
	const std::map<int, std::string>& getErrorPages() const;

	void setPath(const std::string& path);
	void setAllowedMethods(const std::vector<std::string>& methods);
	void setRoot(const std::string& root);
	void setIndex(const std::string& index);
	void setUploadDir(const std::string& uploadDir);
	void setAutoIndex(bool autoIndex);
	void setClientMaxBodySize(int size);
	void setCgiExtensions(const std::vector<std::string>& extensions);
	void setCgiPath(const std::string& cgiPath);
	void setRedirectCode(int code);
	void setRedirectUrl(const std::string& url);
	void setErrorPages(const std::map<int, std::string>& errorPages);
	void addAllowedMethod(const std::string& method);
	void addCgiExtension(const std::string& extension);
	void addErrorPage(int code, const std::string& page);
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

	bool _default;

public:
	Server(std::vector<std::string> confFile);
	~Server();

	std::string getServerName() const;
	std::string getHost() const;
	int getPort() const;
	std::string getRoot() const;
	std::string getIndex() const;
	int getClientMaxBodySize() const;
	const std::map<int, std::string> getErrorPages() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::map<std::string, Location*>& getLocations() const;
	int getMaxBodySize() const;
	bool getAutoIndex() const;
	std::map<int, std::string> getErrorPages();

	bool getDefaultStatus();

	void setServerName(const std::string& server);
	void setHost(std::string& hostname);
	void setPort(int& porttmp);
	void setRoot(std::string& rootname);
	void setIndex(std::string& indexname);
	void addErrorPage(int err, std::string mess);
	void addAllowedMethod(std::string method);
	void setClientMaxBody(int clientMaxBody);
	void setLocation(const std::string, Location *location);

	void setDefaultServer();
};
