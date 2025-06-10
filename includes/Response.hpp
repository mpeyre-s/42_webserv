#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

#include "../includes/Server.hpp"

class Request;

class Response {
private:
	Request *_obj;
	std::vector<Server*> _list_servers;

	// Response
	std::string _http_version;
	int _status;
	std::string _text_status;
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	Response(Request *obj, std::vector<Server*> list_servers, int status);
	~Response();

	std::string getStringResponse();
};
