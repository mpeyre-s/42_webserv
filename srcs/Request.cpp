#include "../includes/Request.hpp"
#include "../includes/Response.hpp"

Request::Request(std::string &raw, std::vector<char> vec, std::vector<Server*> list_servers) {
	_vec_char = vec;
	_list_servers = list_servers;
	_parsing_error = 200;
	std::vector<std::string> lines = split(raw, "\r\n");

	// parsing request line (1st line)
	std::vector<std::string> request_line_splited = split(lines[0], " ");
	if (request_line_splited.size() != 3) {
		_parsing_error = 400;
		return;
	}
	if (request_line_splited[0] != "GET" && request_line_splited[0] != "POST" && request_line_splited[0] != "DELETE") {
		_parsing_error = 400;
		return;
	}
	_method_type = request_line_splited[0];
	if (request_line_splited[1].empty() || request_line_splited[1][0] != '/') {
		_parsing_error = 400;
		return;
	}
	_path_to_resource = request_line_splited[1];
	if (request_line_splited[2] != "HTTP/1.1") {
		_parsing_error = 400;
		return;
	}
	_http_version = request_line_splited[2];

	_const_http_version = _http_version;
	_const_method_type = _method_type;
	_const_path_to_resource = _path_to_resource;

	// headers parsing (map)
	size_t body_start_pos = 2;
	for (size_t i = 1; lines[i].length() != 1; i++) {
		size_t start_pos = 0;
		size_t end_pos = lines[i].find(":");
		size_t end_string_pos = lines[i].length();
		std::string key = lines[i].substr(start_pos + 1, end_pos - 1);
		std::string value = lines[i].substr(end_pos + 2, end_string_pos - 1);
		_headers[key] = value;
		body_start_pos++;
	}

	// body parsing raw -> next double \r\n
	if (body_start_pos == lines.size())
		return;

	for (size_t i = body_start_pos; i < lines.size(); i++) {
		if (i == body_start_pos) {
			_body = lines[i].substr(1, lines[i].length());
		} else
			_body.append(lines[i]);
	}
}

Response* Request::process(Server* server) {
	std::string log = _const_method_type + " " + _const_path_to_resource + " " + _const_http_version;
	std::cout << "\033[35m[TRACE] " << log << "\033[0m" << std::endl;
	Request *request_copy = new Request(*this);
	return new Response(request_copy, server, _parsing_error, _list_servers);
}

Request &Request::operator=(const Request &other) {
	if (this != &other) {
		_parsing_error = other._parsing_error;
		_method_type = other._method_type;
		_path_to_resource = other._path_to_resource;
		_http_version = other._http_version;
		_headers = other._headers;
		_body = other._body;
	}
	return *this;
}

Request::~Request() {
	_headers.clear();
}

std::string Request::getMethodType() const {
	return _method_type;
}

std::string Request::getPathToResource() const {
	return _path_to_resource;
}

std::vector<char> Request::getVecChar() {
	return _vec_char;
}

std::map<std::string, std::string> Request::getHeaders() const {
	return _headers;
}

bool	Request::isKeepAlive() {
	if (_headers.find("Connection") != _headers.end())
	{
		if (_headers["Connection"] == "keep-alive")
			return true ;
	}
	return false ;
}

std::string Request::getBody() const {
	return _body;
}

std::string	Request::getContentType()
{
	if (_headers.find("Content-Type") != _headers.end())
		 return (_headers["Content-Type"]);
	else
		return ("error");
}

void	Request::setPathToResource(std::string &new_path) {
	_path_to_resource = new_path;
}

