#include "../includes/Response.hpp"
#include "../includes/Request.hpp"

static std::string getContentTypeFromPath(std::string &path) {
	const char *extensions[] = {".html", ".css", ".js", ".json", ".xml", ".bin", ".exe", ".dll", ".jpg", ".jpeg", ".png", ".gif", ".mp3", ".mp4", ".pdf", ".zip", ".txt", NULL};
	for (size_t i = 0; extensions[i]; i++) {
		size_t pos = path.find(extensions[i]);
		if (pos != std::string::npos && (pos + strlen(extensions[i]) == path.length())) {
			if (strcmp(extensions[i], ".html") == 0) {
				return "text/html";
			} else if (strcmp(extensions[i], ".css") == 0) {
				return "text/css";
			} else if (strcmp(extensions[i], ".js") == 0) {
				return "application/javascript";
			} else if (strcmp(extensions[i], ".json") == 0) {
				return "application/json";
			} else if (strcmp(extensions[i], ".xml") == 0) {
				return "application/xml";
			} else if (strcmp(extensions[i], ".bin") == 0 || strcmp(extensions[i], ".exe") == 0 || strcmp(extensions[i], ".dll") == 0) {
				return "application/octet-stream";
			} else if (strcmp(extensions[i], ".jpg") == 0 || strcmp(extensions[i], ".jpeg") == 0) {
				return "image/jpeg";
			} else if (strcmp(extensions[i], ".png") == 0) {
				return "image/png";
			} else if (strcmp(extensions[i], ".gif") == 0) {
				return "image/gif";
			} else if (strcmp(extensions[i], ".mp3") == 0) {
				return "audio/mpeg";
			} else if (strcmp(extensions[i], ".mp4") == 0) {
				return "video/mp4";
			} else if (strcmp(extensions[i], ".pdf") == 0) {
				return "application/pdf";
			} else if (strcmp(extensions[i], ".zip") == 0) {
				return "application/zip";
			} else if (strcmp(extensions[i], ".txt") == 0) {
				return "text/plain";
			}
		}
	}
	return "text/brut";
}

static bool pathIsFile(std::string &path) {
	if (path[path.length() - 1] == '/')
		return false;
	return true;
}

static bool isPathOpenable(std::string &path) { // il faut fermer le fd ! /\/\/\/\/\/\//\/
	if (pathIsFile(path)) {
		int fd = open(path.c_str(), O_RDONLY);
		if (fd == -1)
			return false;
		return true;
	} else {
		DIR *pDir = opendir(path.c_str());
		if (pDir == NULL)
			return false;
		return true;
	}
}

static std::string intToStdString(int nb) {
	std::ostringstream oss117;
	oss117 << nb;
	return oss117.str();
}

static int getFileOctetsSize(std::string &path) {
	struct stat file_stat;

	if (stat(path.c_str(), &file_stat) == 0)
		return file_stat.st_size;
	return 0;
}

static std::string pathfileToStringBackslashs(std::string &path) {
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

static std::string getDate() {
	std::time_t now = std::time(0);
	struct tm *LT = std::localtime(&now);

	std::string week_day;
	const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	week_day = weekdays[LT->tm_wday % 7];

	std::string month_day = intToStdString(LT->tm_mday);

	std::string month;
	const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	month = months[LT->tm_mon];

	std::string year = intToStdString(LT->tm_year + 1900);

	std::string time = intToStdString(LT->tm_hour) + ":" + intToStdString(LT->tm_min) + ":" + intToStdString(LT->tm_sec);

	return week_day + ", " + month_day + " " + month + " " + year + " " + time + " GMT";
}

Response::Response(Request *request, Server* server, int status) : _request(request), _server(server), _status(status) {
	// default html error pages
	bad_request_path = "resources/bad_request.html";
	internal_server_error_path = "resources/internal_server_error.html";
	not_found_path = "resources/not_found.html";
	auto_index_path = "resources/auto_index.html";
	request_entity_too_large_path = "resources/request_entity_too_large.html";

	// default params
	_http_version = "HTTP/1.1";
	_headers["Date"] = getDate();
	_headers["Server"] = "Webserv/1.0 (42)";
	if (_request->isKeepAlive() == false)
		_headers["Connection"] = "Closed";
	else
		_headers["Connection"] = "keep-alive";

	// get server params based on path to ressource request
	potential_server = "/";
	std::map<std::string, Location> locations_nested = _server->getLocations();
	size_t max_match_length = 0;
	for (std::map<std::string, Location>::iterator it = locations_nested.begin(); it != locations_nested.end(); ++it) {
		std::string key = it->first;
		if (strncmp(key.c_str(), _request->getPathToResource().c_str(), key.length()) == 0) {
			if (key.length() > max_match_length) {
				potential_server = key;
				max_match_length = key.length();
			}
		}
	}
	std::cout << "Potential Server: " << potential_server << std::endl;

	// class instant with all config params for the current endpoint asked
	cur_location = locations_nested[potential_server];
	if (potential_server == "/" || potential_server.empty())
		cur_location.path = "/";
	if (cur_location.path.empty())
		cur_location.path = potential_server;
	if (cur_location.allowed_methods.empty())
		cur_location.allowed_methods = server->getAllowedMethods();
	if (cur_location.root.empty())
		cur_location.root = server->getRoot();
	if (cur_location.index.empty())
		cur_location.index = server->getIndex();
	if (!cur_location.client_max_body_size)
		cur_location.client_max_body_size = server->getClientMaxBodySize();
	if (cur_location.error_pages.size() == 0)
		cur_location.error_pages = server->getErrorPages();

	// replace error pages if needed
	if (cur_location.error_pages.find(400) != cur_location.error_pages.end())
		bad_request_path = cur_location.error_pages[400];
	if (cur_location.error_pages.find(413) != cur_location.error_pages.end())
		request_entity_too_large_path = cur_location.error_pages[413];
	if (cur_location.error_pages.find(500) != cur_location.error_pages.end())
		internal_server_error_path = cur_location.error_pages[500];
	if (cur_location.error_pages.find(404) != cur_location.error_pages.end())
		not_found_path = cur_location.error_pages[404];

	//check if status is bad request
	if (_status != 200)
		_badRequest = true;
	else
		_badRequest = false;

}

// bad request + internal server error + Request Entity Too Large
void	Response::badRequest()
{
	if (_status == 400) {
		_text_status = "Bad Request";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(bad_request_path));
		_body = pathfileToStringBackslashs(bad_request_path);
		return;
	}
	else if ((int)_request->getBody().size() > cur_location.client_max_body_size) {
		_status = 413;
		_text_status = "Request Entity Too Large";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(request_entity_too_large_path));
		_body = pathfileToStringBackslashs(request_entity_too_large_path);
		return;
	}
	else if (_status != 200) {
		_status = 500;
		_text_status = "Internal Server Error";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(internal_server_error_path));
		_body = pathfileToStringBackslashs(internal_server_error_path);
		return;
	}
}

void	Response::setPath()
{
	_text_status = "OK";

	path = cur_location.root + _request->getPathToResource().substr(cur_location.path.length());
	if (path[path.length() - 1] == '/' && cur_location.auto_index == false)
		path.append(cur_location.index);
	else if (_request->getPathToResource() == potential_server)
		path.append("/");
	std::cout << "PATH: " << path << std::endl;

	//check if path is correct
	if (isPathOpenable(path) == false)
	{
		_status = 404;
		_text_status = "Not Found";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(not_found_path));
		_body = pathfileToStringBackslashs(not_found_path);
		_correctPath = false;
	}
}

void	Response::get()
{
	if (_correctPath)
	{
		if (pathIsFile(path))
		{
			_headers["Content-Type"] = getContentTypeFromPath(path);
			_headers["Content-Length"] = intToStdString(getFileOctetsSize(path));
			_body = pathfileToStringBackslashs(path);
			return;
		}
		else
		{
			_headers["Content-Type"] = getContentTypeFromPath(auto_index_path);
			_headers["Content-Length"] = intToStdString(getFileOctetsSize(auto_index_path));
			_body = pathfileToStringBackslashs(auto_index_path);
			return;
		}
	}
}


void	Response::parseBody(std::string body)
{
}

void	Response::post()
{
	setPath();
	if (_correctPath)
	{
		if (pathIsFile(path))
		{
			_headers["Content-Type"] = getContentTypeFromPath(path);
			_headers["Content-Length"] = intToStdString(getFileOctetsSize(path));
		}
		else
		{
			_headers["Content-Type"] = getContentTypeFromPath(auto_index_path);
			_headers["Content-Length"] = intToStdString(getFileOctetsSize(auto_index_path));
		}
		parseBody(_request->getBody());
	}
	return;
}

void	Response::Delete() {
	_headers["Content-Type"] = "text/brut";
	_headers["Content-Length"] = "4";
	_body = "OK\r\n";
	return;
}

void	Response::process() {
	if (_badRequest)
		badRequest();
	else if (_request->getMethodType() == "GET")
		get();
	else if (_request->getMethodType() == "POST")
		post();
	else if (_request->getMethodType() == "DELETE")
		Delete();
}

std::string Response::getStringResponse() {
	// build string -> buffer_out
	process();
	std::string result;
	result.append(_http_version + " " + intToStdString(_status) + " " + _text_status + "\r\n");
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		result.append(it->first + ": " + it->second +  "\r\n");
	}
	result.append("\r\n");
	result.append(_body);
	return result;
}

Response::~Response() {}
