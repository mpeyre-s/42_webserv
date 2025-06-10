#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "Server.hpp"

class Response;

class Request {
private:
	bool _parsing_error;

	// REQUEST
	std::string _method_type;
	std::string _path_to_resource;
	std::string _http_version;
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	Request(std::string &raw);
	~Request();

	Request &operator=(const Request &other);

	Response *process(std::vector<Server*> list_servers);

	std::string getMethodType() const;
	std::string getPathToResource() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
};
