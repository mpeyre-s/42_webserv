#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "Server.hpp"

class Response;

class Request {
private:
	int _parsing_error;

	// REQUEST
	std::string _method_type;
	std::string _path_to_resource;
	std::string _http_version;
	std::map<std::string, std::string> _headers;  //  il va falloir qu'on se serve du Host ici pour verifier le serveur
	std::string _body;
	std::vector<char> _vec_char;
	size_t	_bufferLen;

	std::string _const_method_type;
	std::string _const_path_to_resource;
	std::string _const_http_version;

public:
	Request(std::string &raw, std::vector<char>, size_t); // ballec de size_t par contre il faudrait envoyer le vecteur de serveur pour faire la verif
	~Request();

	Request &operator=(const Request &other);

	Response *process(Server* server);

	std::string getMethodType() const;
	std::string getPathToResource() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
	std::string	getContentType();
	std::vector<char> getVecChar();
	size_t getBufferLen();

	void setPathToResource(std::string &new_path);

	bool	isKeepAlive();
};
