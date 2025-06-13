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
	Location cur_location;

	// Response
	int			_status;
	bool		_badRequest;
	std::string	_text_status;
	std::string	_http_version;
	std::map<std::string, std::string> _headers;
	std::string _body;

	//default html error page
	std::string bad_request_path;
	std::string internal_server_error_path;
	std::string not_found_path;
	std::string auto_index_path;
	std::string request_entity_too_large_path;

	// server params
	std::string potential_server;
	std::string path;
	bool		_correctPath;

	void	setPath();
	void	parseBody(std::string);
	void	parseUrlEncodedBody(std::string); //TODO
	void	parseJsonBody(std::string); //TODO

public:
	Response(Request *request ,Server* server, int status);
	~Response();

	std::string getStringResponse();
	void	process();
	void	badRequest();
	void	get();
	void	post();
	void	Delete();
};
