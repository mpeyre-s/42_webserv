#include "../includes/Response.hpp"
#include "../includes/Request.hpp"

static std::vector<std::string> generateListingDirectory(std::string &path) {
	std::vector<std::string> result;
	DIR *dir;
	dir = opendir(path.c_str());
	if (dir != nullptr) {
		struct dirent *ent;
		size_t i = -1;
		while ((ent = readdir(dir)) != nullptr) {
			if (++i > 1)
				result.push_back(ent->d_name);
		}
	}
	return result;
}

static std::string generateAutoIndexHtml(const std::string &path, const std::string &request_ressource) {
	std::vector<std::string> ls = generateListingDirectory(const_cast<std::string&>(path));
	std::string result;
	result = "<html>\n<head>\n<title>Index Of " + request_ressource + "</title>\n</head>\n<body>\n<h1>Index Of " + request_ressource + "</h1>\n<ul>\n";
	result.append("<li><a href=\"../\">Parent Directory</a></li>\n");
	size_t nb_elements = ls.size();
	for (size_t i = 0; i < nb_elements; i++) {
		result.append("<li><a href=\"" + request_ressource + "/" + ls[i] + "\">" + ls[i] + "</a></li>\n");
	}
	result.append("</ul>\n</body>\n</html>");
	return result;
}

std::string Response::getContentTypeFromPath(std::string &path) {
	for (std::map<std::string, std::string>::iterator it = _file_types.begin(); it != _file_types.end(); it++) {
		if (path.length() >= it->first.length() && path.compare(path.length() - it->first.length(), it->first.length(), it->first) == 0) {
			return it->second;
		}
	}
	return "text/brut";
}

static bool pathIsFile(std::string &path) {
	if (path[path.length() - 1] == '/')
		return false;
	return true;
}

static bool isPathOpenable(std::string &path) {
	if (pathIsFile(path)) {
		int fd = open(path.c_str(), O_RDONLY);
		if (fd == -1)
			return false;
		close(fd);
		return true;
	} else {
		DIR *pDir = opendir(path.c_str());
		if (pDir == NULL)
			return false;
		closedir(pDir);
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

static bool isPathFileBinary(std::string &path) {
	const char* binaryExtensions[] = {".bin", ".exe", ".dll", ".jpg", ".jpeg", ".png", ".svg", ".gif", ".mp3", ".mp4", ".pdf", ".zip", NULL};
	for (int i = 0; binaryExtensions[i]; i++) {
		size_t pos = path.find(binaryExtensions[i]);
		if (pos != std::string::npos && (pos + strlen(binaryExtensions[i]) == path.length())) {
			return true;
		}
	}
	return false;
}

static std::string pathfileToStringBackslashs(std::string &path) {
	std::string result;

	if (isPathFileBinary(path)) {
		std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
		if (!file.is_open())
			return "";

		file.seekg(0, std::ios::end);
		std::streamoff size = file.tellg();
		file.seekg(0, std::ios::beg);

		if (size > 0) {
			char* buffer = new char[size];
			file.read(buffer, size);
			result.assign(buffer, size);
			delete[] buffer;
		}
		file.close();
	} else {
		std::ifstream inputFile(path.c_str());
		if (!inputFile.is_open())
			return "";

		std::string line;
		while (std::getline(inputFile, line)) {
			result.append(line + "\n");
		}
		inputFile.close();
	}
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
	request_entity_too_large_path = "resources/request_entity_too_large.html";

	// default params
	_http_version = "HTTP/1.1";
	_headers["Date"] = getDate();
	_headers["Server"] = "Webserv/1.0 (42)";
	if (_request->isKeepAlive() == false)
		_headers["Connection"] = "close";
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

	_file_types[".html"] = "text/html";
	_file_types[".css"] = "text/css";
	_file_types[".js"] = "application/javascript";
	_file_types[".json"] = "application/json";
	_file_types[".xml"] = "application/xml";
	_file_types[".svg"] = "image/svg+xml";
	_file_types[".bin"] = "application/octet-stream";
	_file_types[".exe"] = "application/octet-stream";
	_file_types[".dll"] = "application/octet-stream";
	_file_types[".jpg"] = "image/jpeg";
	_file_types[".jpeg"] = "image/jpeg";
	_file_types[".png"] = "image/png";
	_file_types[".gif"] = "image/gif";
	_file_types[".mp3"] = "audio/mpeg";
	_file_types[".mp4"] = "video/mp4";
	_file_types[".pdf"] = "application/pdf";
	_file_types[".zip"] = "application/zip";
	_file_types[".txt"] = "text/plain";
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
	_correctPath = true;

	path = cur_location.root + _request->getPathToResource().substr(cur_location.path.length());
	if (path[path.length() - 1] == '/' && cur_location.auto_index == false)
		path.append(cur_location.index);
	else if (_request->getPathToResource() == potential_server)
		path.append("/");

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

// ================================ Toute la logique de get =================================

void	Response::get()
{
	setPath();
	if (!_correctPath)
		return;

	if (cur_location.redirect_code != 0 && !cur_location.redirect_url.empty()) {
		_status = 301;
		_headers["Location"] = cur_location.redirect_url;
		return;
	}

	if (pathIsFile(path))
	{
		_headers["Content-Type"] = getContentTypeFromPath(path);
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(path));
		_body = pathfileToStringBackslashs(path);
		return;
	}
	else
	{
		_body = generateAutoIndexHtml(path, _request->getPathToResource());
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(_body.length());
		return;
	}
}

// ================================ Toute la logique de post =================================

std::string	Response::checkHeader()
{
	std::string ContentType = _request->getContentType();
	std::cout << "Mon content type est ici : " << ContentType << std::endl;
	size_t pos = ContentType.find("multipart/form-data");
	if (pos == std::string::npos)
	{
		_status = 415;
		badRequest();
		return "error";
	}
	std::string boundary = "boundary=";
	size_t poss = ContentType.find(boundary);
	if (poss == std::string::npos)
	{
		_status = 415;
		badRequest();
		return "error";
	}

	size_t start = poss + boundary.length();
	size_t end = ContentType.find(';', start);
	if (end == std::string::npos)
		end = ContentType.length();

	std::string real_boundary = "--" + ContentType.substr(start, end - start);

	return real_boundary;
}

void Response::parsePostHeader(std::istringstream &iss, std::string &line)
{
	std::cout << "===== headers détecté ======" << std::endl;
	while (std::getline(iss, line) && line != "\r" && !line.empty()) // faut peut etre check si il y a 0 header
	{
		if (line.find("Content-Disposition:") != std::string::npos)
		{
			size_t start = line.find("filename=\"");
			if (start != std::string::npos)
				start += 10;
			size_t end = line.find('"', start);
			_filename = line.substr(start, end - start);
			std::cout << "filename = " << _filename << std::endl;
		}
		else if (line.find("Content-Type:") != std::string::npos)
		{
			size_t typeStart = line.find(":") + 2;
			_contentType = line.substr(typeStart);
			std::cout << "Content type = " << _contentType << std::endl;
		}
	}
}

std::string	Response::checkExtension()
{
	for (std::map<std::string, std::string>::iterator it = _file_types.begin(); it != _file_types.end(); ++it)
	{
		if (it->second == _contentType)
			return (it->first);
	}
	return NULL;
}

void		Response::parseBodyBinary(std::istringstream &iss, std::string &line)
{
	(void)iss;
	(void)line;
}

void		Response::parseBodyText(std::istringstream &iss, std::string &line)
{
	std::string file_path = _server->getUploadDir() + "/" + _filename;
	std::cout << "Le path ou sera televerser le fichier est : " << file_path << std::endl;
	std::ofstream outfile(file_path.c_str());
	if (!outfile.is_open()) {
		_status = 500;
		badRequest();
		std::cerr << "Error writing file" << std::endl; // il faut mieux le gerer
		return;
	}

	//std::getline(iss, line); // sauter la ligne entre header et body apparement pas besoin

	while (std::getline(iss, line))
	{
		if (line == _boundary + "--")
			break;
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::cout << "Ligne prise du body : " << line << std::endl;
		outfile << line << "\n";
	}
	std::cout << "You see me?" << std::endl;
}

void	Response::parseBody(std::string body)
{
	_boundary = checkHeader();
	if (_boundary == "error")
		return ;
	std::istringstream iss(body);
	std::string line;
	while (std::getline(iss, line))
	{
		if (line == _boundary)
		{
			// 1. Lire headers
			parsePostHeader(iss, line);

			// 2. Lire le body
			std::string extension = checkExtension();
			if (extension.empty()) {
				_status = 415;
				badRequest();
				return ;
			}
			if (isPathFileBinary(extension))
				parseBodyBinary(iss, line);
			else
				parseBodyText(iss, line);
		}
		if (line == _boundary + "--") {
			std::cout << "finish" << std::endl;
			break ;
		}
	}
	std::cout << "out of parse body" << std::endl;
}

void	Response::post()
{
	setPath();
	if (!_correctPath)
		return ;
	parseBody(_request->getBody());

	std::cout << "le statut est : " << _status << std::endl;
	_status = 201;
	_text_status = "Created";
	_body = "Upload successful.\n";
	_headers["Content-Type"] = "text/plain";
	_headers["Content-Length"] = intToStdString(_body.length());
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
	//std::cout << "status = " << _status << std::endl << "text status = " << _text_status << std::endl << "body = " << _body << std::endl;
	result.append(_http_version + " " + intToStdString(_status) + " " + _text_status + "\r\n");
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		result.append(it->first + ": " + it->second +  "\r\n");
	}
	result.append("\r\n");
	result.append(_body);
	return result;
}

Response::~Response() {}


// <html>
// 	<head>
// 		<title>415 Unsupported Media Type</title>
// 	</head>
// 	<body>
// 		<h1>415 Unsupported Media Type</h1>
// 		<p>The media type provided in the request is not supported by this server.</p>
// 	</body>
// </html>

