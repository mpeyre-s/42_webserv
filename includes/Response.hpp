#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>

#include "Server.hpp"
#include "utils.hpp"

static const size_t BUFFER_SIZES = 2048;

#define ABSOLUTE_PATH "/Users/spike/42/WebServ/"
#define PHP_PATH "/opt/homebrew/opt/php/bin/php-cgi"

class Request;

class Response {
private:
	Request* _request;
	Server* _server;
	Location* cur_location;
	bool _ownLocation;


	// Response
	int			_status;
	bool		_badRequest;
	std::string	_text_status;
	std::string	_http_version;
	std::map<std::string, std::string> _headers;
	std::vector<Server *> _list_servers;
	std::string	_filename;
	std::string	_contentType;
	std::string _body;

	//default html error page
	std::string bad_request_path;
	std::string internal_server_error_path;
	std::string not_found_path;
	std::string auto_index_path;
	std::string request_entity_too_large_path;
	std::string unsuported_media_path;
	std::string forbidden_path;
	std::string method_not_allowed_path;
	std::string moved_permanently_path;

	std::map<std::string, std::string> _file_types; // ext -> content type

	// server params
	std::string potential_server;
	std::string path;
	std::string _boundary;
	bool		_correctPath;
	std::vector<std::string> _env;
	bool		_isphp;
	bool		_isCGI;

	std::string	checkHeader();
	void		parsePostHeader(std::istringstream& iss, std::string& line);
	void		parseBodyBinary(std::vector<char>);
	void		parseBodyText(std::istringstream& iss, std::string& line);
	void		setPath();
	void		parseBody(std::string);
	std::string	getContentTypeFromPath(std::string &path);
	std::string	checkExtension();
	bool		isValidMethodExtension();
	bool		isValidCgi();
	void		create_env();
	void		parseCgiBody(std::string cgi_body);
	void		getCGI(std::string, std::vector<char *>);
	void		postCGI(std::string, std::vector<char *>);


public:
	Response(Request *request ,Server* server, int status, std::vector<Server *>);
	~Response();

	std::string getStringResponse();
	void	process();
	void	badRequest();
	void	get();
	void	post();
	void	Delete();
	void	cgi();

};
