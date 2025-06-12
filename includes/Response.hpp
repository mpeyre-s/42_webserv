#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>

#include "../includes/Server.hpp"

class Request;

class Response {
private:
	Request* _request;
	Server* _server;

	// Response
	std::string _http_version;
	int _status;
	std::string _text_status;
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	Response(Request *request ,Server* server, int status);
	~Response();

	std::string getStringResponse();
};
