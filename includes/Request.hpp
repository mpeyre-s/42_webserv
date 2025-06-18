#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "Server.hpp"
#include "utils.hpp"

class Response;

class Request {
private:
	int _parsing_error;

	// REQUEST
	std::string _method_type;
	std::string _path_to_resource;
	std::string _http_version;
	std::map<std::string, std::string> _headers;
	std::string _body;

	std::string _const_method_type;
	std::string _const_path_to_resource;
	std::string _const_http_version;

public:
	Request(std::string &raw);
	~Request();

	Request &operator=(const Request &other);

	Response *process(Server* server);

	std::string getMethodType() const;
	std::string getPathToResource() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
	std::string	getContentType();

	void setPathToResource(std::string &new_path);

	bool	isKeepAlive();
};
