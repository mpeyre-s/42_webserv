#include "../includes/Response.hpp"
#include "../includes/Request.hpp"

static std::string intToStdString(int nb) {
	std::ostringstream oss;
	oss << nb;
	return oss.str();
}

static int getFileOctetsSize(std::string path) {
	struct stat file_stat;

	if (stat(path.c_str(), &file_stat) == 0)
		return file_stat.st_size;
	return 0;
}

static std::string pathfileToStringBackslashs(std::string path) {
	std::string result;
	std::ifstream inputFile(path.c_str());

	if (!inputFile.is_open())
		return "";

	std::string line;
	while (std::getline(inputFile, line)) {
		result.append(line + "\r\n");
	}

	inputFile.close();
	return result;
}

Response::Response(Request *obj, std::vector<Server*> list_servers, int status) : _obj(obj), _list_servers(list_servers), _status(status) {
	_http_version = "HTTP/1.1";
	std::string bad_request_path = "resources/bad_request.html";

	if (_status == 400) {
		_text_status = "Bad Request";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(bad_request_path));
		_headers["Connection"] = "Closed";
		_body = pathfileToStringBackslashs(bad_request_path);
	} else {
		_text_status = "OK";
		_headers["Content-Type"] = "text/brut";
		_body = "" + _obj->getBody();
	}
}

std::string Response::getStringResponse() {
	std::string result;

	// 1st line
	result.append(_http_version + " " + intToStdString(_status) + " " + _text_status + "\r\n");

	// headers
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		result.append(it->first + ": " + it->second +  "\r\n");
	}

	// body
	result.append("\r\n");
	result.append(_body);

	return result;
}

Response::~Response() {}
