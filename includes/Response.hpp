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

#include <iomanip> // pour std::hex et std::setw

#include "../includes/Server.hpp"

static const size_t BUFFER_SIZES = 2048;

#define ABSOLUTE_PATH "/Users/spike/42/WebServ/"
#define PHP_PATH "/opt/homebrew/opt/php/bin/php-cgi"

class Request;

class Response {
private:
	Request* _request;
	Server*	_server;
	Location cur_location;


	// Response
	int			_status;
	bool		_badRequest;
	std::string	_text_status;
	std::string	_http_version;
	std::map<std::string, std::string> _headers;
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
	void		parseBodyBinary(std::vector<char>, size_t);
	void		parseBodyText(std::istringstream& iss, std::string& line);
	void		setPath();
	void		parseBody(std::string);
	std::string	getContentTypeFromPath(std::string &path);
	std::string	checkExtension();
	bool		isCGI();
	bool		isValidCgi();
	void		create_env();
	void		execCGI(std::string, std::vector<char *>);
	void		parseCgiBody(std::string cgi_body);


public:
	Response(Request *request ,Server* server, int status);
	~Response();

	std::string getStringResponse();
	void	process();
	void	badRequest();
	void	get();
	void	post();
	void	Delete();
	void	cgi();

};
