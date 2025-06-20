#include "../includes/Response.hpp"
#include "../includes/Request.hpp"

static std::vector<std::string> generateListingDirectory(std::string &path) {
	std::vector<std::string> result;
	DIR *dir = opendir(path.c_str());
	if (dir != NULL) {
		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL) {
			std::string name = ent->d_name;
			if (name != "." && name != "..")
				result.push_back(name);
		}
		closedir(dir);
	}
	return result;
}

static std::string generateAutoIndexHtml(const std::string &path, const std::string &request_ressource) {
	std::vector<std::string> ls = generateListingDirectory(const_cast<std::string&>(path));
	std::string result;
	result = "<html>\n<head>\n<title>Index Of " + request_ressource + "</title>\n</head>\n<body>\n<h1>Index Of " + request_ressource + "</h1>\n<ul>\n";
	result.append("<li><a href=\"../\">Parent Directory</a></li>\n");
	for (size_t i = 0; i < ls.size(); i++) {
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

Response::Response(Request *request, Server* server, int status, std::vector<Server *> list_serv) : _request(request), _server(server), _status(status), _list_servers(list_serv) {
	// default html error pages
	bad_request_path = "resources/bad_request.html";
	internal_server_error_path = "resources/internal_server_error.html";
	not_found_path = "resources/not_found.html";
	request_entity_too_large_path = "resources/request_entity_too_large.html";
	unsuported_media_path = "resources/unsuported_media_path.html";
	forbidden_path = "resources/forbidden.html";
	method_not_allowed = "resources/method_not_allowed.html";
	moved_permanently_path = "resources/moved_permanently.html";

	// default params
	_http_version = "HTTP/1.1";
	_headers["Date"] = getDate();
	_headers["Server"] = "Webserv/1.0 (42)";
	if (_request->isKeepAlive() == false)
		_headers["Connection"] = "close";
	else
		_headers["Connection"] = "keep-alive";


	// Logic to find the correct server
	std::map<std::string, std::string> host_map = _request->getHeaders();
	std::string fullhost = "";
	std::string host = "";
	int port = -1;

	if (host_map.find("Host") != host_map.end()) {
		fullhost = host_map["Host"];
		size_t end_host = fullhost.find(':');
		if (end_host != std::string::npos) {
			host = fullhost.substr(0, end_host);
			port = std::atoi(fullhost.substr(end_host + 1).c_str());
		}
		else {
			host = fullhost;
		}
	}

	for (std::vector<Server *>::const_iterator it = _list_servers.begin() ; it != _list_servers.end(); ++it)
	{
		if (host == (*it)->getServerName() && port == (*it)->getPort())
		{
			_server = *it;
			break;
		}
	}


	// get server params based on path to ressource request
	potential_server = "/";
	std::map<std::string, Location*> locations_nested = _server->getLocations();
	size_t max_match_length = 0;
	for (std::map<std::string, Location*>::iterator it = locations_nested.begin(); it != locations_nested.end(); ++it) {
		std::string key = it->first;
		if (strncmp(key.c_str(), _request->getPathToResource().c_str(), key.length()) == 0) {
			if (key.length() > max_match_length) {
				potential_server = key;
				max_match_length = key.length();
			}
		}
	}

	// class instant with all config params for the current endpoint asked
	std::map<std::string, Location*>::iterator it = locations_nested.find(potential_server);
	if (it != locations_nested.end() && it->second != NULL) {
		cur_location = it->second;
		_ownLocation = false;
	}
	else {
		cur_location = new Location();
		 _ownLocation = true;
	}

	if (potential_server == "/" || potential_server.empty())
		cur_location->setPath("/");
	if (cur_location->getPath().empty())
		cur_location->setPath(potential_server);
	if (cur_location->getAllowedMethods().empty())
		cur_location->setAllowedMethods(server->getAllowedMethods());
	if (cur_location->getRoot().empty())
		cur_location->setRoot(server->getRoot());
	if (cur_location->getIndex().empty())
		cur_location->setIndex(server->getIndex());
	if (cur_location->getMaxBodySize() < 0)
		cur_location->setClientMaxBodySize(server->getMaxBodySize());
	if (cur_location->getErrorPages().size() == 0)
		cur_location->setErrorPages(server->getErrorPages());


	// replace error pages if needed
	std::map<int, std::string> current_error_pages = cur_location->getErrorPages();
	if (cur_location->getErrorPages().find(400) != cur_location->getErrorPages().end())
		bad_request_path = current_error_pages[400];
	if (cur_location->getErrorPages().find(413) != cur_location->getErrorPages().end())
		request_entity_too_large_path = current_error_pages[413];
	if (cur_location->getErrorPages().find(500) != cur_location->getErrorPages().end())
		internal_server_error_path = current_error_pages[500];
	if (cur_location->getErrorPages().find(404) != cur_location->getErrorPages().end())
		not_found_path = current_error_pages[404];
	if (cur_location->getErrorPages().find(415) != cur_location->getErrorPages().end())
		unsuported_media_path = current_error_pages[415];
	if (cur_location->getErrorPages().find(403) != cur_location->getErrorPages().end())
		forbidden_path = current_error_pages[403];


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
	else if (_status == 403)
	{
		_text_status = "Forbidden";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(forbidden_path));
		_body = pathfileToStringBackslashs(forbidden_path);
	}
	else if (_status == 404)
	{
		_text_status = "Not Found";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(not_found_path));
		_body = pathfileToStringBackslashs(not_found_path);
	}
	else if (_status == 405)
	{
		_text_status = "Method not Allowed";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(forbidden_path));
		_body = pathfileToStringBackslashs(forbidden_path);
	}
	else if (_status == 415) {
		_text_status = "Unsupported Media Type";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(unsuported_media_path));
		_body = pathfileToStringBackslashs(unsuported_media_path);
		return;
	}
	else if (_status == 413) {
		_text_status = "Request Entity Too Large";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(request_entity_too_large_path));
		_body = pathfileToStringBackslashs(request_entity_too_large_path);
		return;
	}
	else if (cur_location && (int)_request->getBody().size() > cur_location->getMaxBodySize()) {
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
	_isCGI = false ;
	if (cur_location->cgi_path.empty() == false && cur_location->cgi_extensions.size() > 0)
	{
		std::string suffix = _request->getPathToResource().substr(cur_location->path.length());
		if (suffix[0] == '/')
			suffix.erase(0, 1);
		path = cur_location->cgi_path + suffix;
		_isCGI = true ;
	}
	else {
		size_t trim = 0;
		if (_request->getPathToResource().substr(cur_location->getPath().length())[0] == '/')
			trim = 1;
		path = cur_location->getRoot() + _request->getPathToResource().substr(cur_location->getPath().length() + trim);
	}
	if (path[path.length() - 1] == '/' && cur_location->getAutoIndex() == false)
		path.append(cur_location->getIndex());

	// compose path with filename for delete method
	if (_request->getMethodType() == "DELETE") {
		size_t i = path.find("?");
		if (i != std::string::npos) {
			path = path.substr(0, i);
			path.append(_request->getPathToResource().substr(_request->getPathToResource().find("=") + 1));
		}
	}

	if (_isCGI)
		return ;

	//check if path is correct
	if (isPathOpenable(path) == false)
	{
		_status = 404;
		badRequest();
		_correctPath = false;
	}
}

// ======================================================= Toute la logique de get ===================================================================

void	Response::get()
{
	if (cur_location->getRedirectCode() != 0 && !cur_location->getRedirectUrl().empty()) {
		_status = 301;
		_headers["Location"] = cur_location->getRedirectUrl();
		_headers["Content-Type"] = getContentTypeFromPath(moved_permanently_path);
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(moved_permanently_path));
		_body = pathfileToStringBackslashs(moved_permanently_path);
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
	while (std::getline(iss, line) && line != "\r" && !line.empty()) // faut peut etre check si il y a 0 header (normalement il y a toujours)
	{
		if (line.find("Content-Disposition:") != std::string::npos)
		{
			size_t start = line.find("filename=\"");
			if (start != std::string::npos)
				start += 10;
			size_t end = line.find('"', start);
			_filename = line.substr(start, end - start);
		}
		else if (line.find("Content-Type:") != std::string::npos)
		{
			size_t typeStart = line.find(":") + 2;
			_contentType = line.substr(typeStart);
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

void		Response::parseBodyBinary(std::vector<char> vec)
{
	std::string file_path = cur_location->getUploadDir() + _filename;
	std::ofstream outfile(file_path.c_str(), std::ios::binary);
	if (!outfile.is_open()) {
		_status = 500;
		badRequest();
		std::cerr << "Error writing file" << std::endl;
		return;
	}
	size_t start = 1;
	int count = 0;
	while (start < vec.size() - 4)
	{
		if (vec[start] == '\r' && vec[start + 1] == '\n' && vec[start + 2] == '\r' && vec[start + 3] == '\n')
			count++;
		if (count == 2)
			break;
		start++;
	}
	start += 4;

	size_t i = start;
	while (i < vec.size())
	{
		if (vec[i] == '\r' && vec[i + 1] == '\n' && vec[i + 2] == '-' && vec[i + 3] == '-' && vec[i + 4] == '-' && vec[i + 5] == '-')
			break ;
		else
			outfile.write(&vec[i], 1);
		i++;
	}
	outfile.close();
}


void		Response::parseBodyText(std::istringstream &iss, std::string &line)
{
	std::string file_path = cur_location->getUploadDir() + _filename;
	std::ofstream outfile(file_path.c_str());
	if (!outfile.is_open()) {
		_status = 500;
		badRequest();
		std::cerr << "Error writing file" << std::endl;
		return ;
	}

	while (std::getline(iss, line))
	{
		if (line == _boundary + "--")
			break;
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		outfile << line << "\n";
	}
	outfile.close();
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
				parseBodyBinary(_request->getVecChar());
			else
				parseBodyText(iss, line);
		}
		if (line == _boundary + "--") {
			break ;
		}
	}
}

void	Response::post()
{

	parseBody(_request->getBody());

	if (_status == 200)
	{
		_status = 201;
		_text_status = "Created";
		_headers["Content-Type"] = "text/plain";
		_body = "Upload successful.\n";
		_headers["Content-Length"] = intToStdString(_body.length());
	}
}


void	Response::Delete()
{

	if (remove(path.c_str()) == 0) {
		_headers["Content-Type"] = "text/plain";
		_body = "Delete successful.\n";
		_headers["Content-Length"] = intToStdString(_body.length());
		return;
	}

	if (path[path.length() - 1] != '/')
		path.append("/");

	if (rmdir(path.c_str()) == 0) {
		_headers["Content-Type"] = "text/plain";
		_body = "Delete successful.\n";
		_headers["Content-Length"] = intToStdString(_body.length());
	}
	else {
		_status = 403;
		badRequest();
	}
}

void	Response::process()
{
	if (_request->getBody().length() > (size_t)cur_location->getMaxBodySize()) {
		_status = 413;
		_badRequest = true;
	}
	if (_badRequest) {
		badRequest();
		return ;
	}
	setPath();
	if (!_correctPath)
		return;
	if (_isCGI)
		cgi();
	else if (_request->getMethodType() == "GET")
		get();
	else if (_request->getMethodType() == "POST")
		post();
	else if (_request->getMethodType() == "DELETE")
		Delete();
	else {
		_status = 405;
		badRequest();
	}
}

std::string Response::getStringResponse() {
	// build string -> buffer_out
	process();

	// log
	std::cout << "  ↳ " << _status << " (" << _text_status << ")" << std::endl;

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

Response::~Response()
{
	if (_ownLocation && cur_location) {
		delete cur_location;
	}
	_file_types.clear();
	_env.clear();
}

bool	Response::isValidMethodExtension()
{
	// Check Method available
	std::string Method = _request->getMethodType();
	std::vector<std::string>::iterator it = cur_location->allowed_methods.begin();

	for (; it != cur_location->allowed_methods.end(); ++it)
	{
		if (*it == Method)
			break ;
	}
	if (it == cur_location->allowed_methods.end()) {
		_status = 405;
		return false ;
	}
	//Check extension available
	size_t pos = path.find('?');
	if (pos == std::string::npos)
		pos = path.size();
	std::string sub_path = path.substr(0, pos);
	for (std::vector<std::string>::iterator it = cur_location->cgi_extensions.begin(); it != cur_location->cgi_extensions.end(); ++it)
	{
		size_t ext_pos = sub_path.find(*it);

		if (ext_pos != std::string::npos)
			return true ;
	}
	_status = 403;
	return false ;
}


bool	Response::isValidCgi()
{
	size_t pos = path.find('?');

	if (pos == std::string::npos)
		pos = path.size();


	// On doit avoir un chemin valide
	std::string check_path = path.substr(0, pos);
	if (!isPathOpenable(check_path)) {
		_status = 404;
		return false;
	}
	//Vérifier que le fichier est exécutable
	struct stat st;
	if (stat(check_path.c_str(), &st) == -1)
	{
		_status = 403;
		return false;
	}
	if (!(st.st_mode & S_IXUSR)) // Vérifie si le propriétaire peut exécuter le fichier
	{
		_status = 403;
		return false;
	}
	return true;
}

void	Response::parseCgiBody(std::string cgi_body)
{
	// Trouver la séparation entre en-têtes et contenu
	size_t header_end = cgi_body.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		header_end = cgi_body.find("\n\n");
		if (header_end == std::string::npos)
		{
			_headers["Content-Type"] = "text/html";
			_body = cgi_body;
			return;
		}
		header_end += 2;
	}
	else
		header_end += 4;

	_body = cgi_body.substr(header_end);

	size_t start = cgi_body.find("Content-Type");
	size_t end = cgi_body.substr(start).find(";");
	_headers["Content-Type"] = cgi_body.substr(start, end);
}


void	Response::getCGI(std::string cgi_path, std::vector<char *> envp)
{
	int	fd[2];
	if (pipe(fd) == -1) {
		_status = 500;
		badRequest();
		std::cerr << "Problem with pipe." << std::endl;
		return ;
	}

	pid_t pid = fork();
	if (pid == -1) {
		_status = 500;
		badRequest();
		std::cerr << "Problem with fork" << std::endl;
		return ;
	}

	if (pid == 0) // <= enfant
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		char *av[3];
		if (_isphp)
		{
			av[0] = const_cast<char*>(PHP_PATH);
			av[1] = const_cast<char *>(cgi_path.c_str());
			av[2] = NULL;
		}
		else
		{
			av[0] = const_cast<char *>(cgi_path.c_str());
			av[1] = NULL;
			av[2] = NULL;
		}
		execve(av[0], av, envp.data());

		std::cout << "CGI execution failed" << std::endl; // Que doit on faire ?
		exit(1);
	}
	else // <= parent
	{
		close(fd[1]);
		char buffer[4096];
		std::string cgi_body;
		ssize_t nb_read = 1;

		while (nb_read) {
			nb_read = read(fd[0], buffer, sizeof(buffer));
			cgi_body.append(buffer, nb_read);
		}
		close(fd[0]);

		int status;
		waitpid(pid, &status, 0);

		_status = 200;
		parseCgiBody(cgi_body);
		_headers["Content-Length"] = intToStdString(_body.size());
	}
}

void	Response::create_env()
{
	std::string method = "REQUEST_METHOD=" + _request->getMethodType();
	size_t pos = path.find('?');
	std::string query;
	if (pos == std::string::npos)
		query = "";
	else
		query = path.substr(pos + 1);
	std::string query_string = "QUERY_STRING=" + query;
	std::string real_path = "SCRIPT_NAME=" + path.substr(0, pos);
	std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";

	_env.push_back(method);
	_env.push_back(query_string);
	_env.push_back(real_path);
	_env.push_back(protocol);

	// Logique POST mais ok de laisser dans GET
	std::map<std::string, std::string> headers = _request->getHeaders();
	std::string content_type = "CONTENT_TYPE=" + headers["Content-Type"];
	std::string content_length = "CONTENT_LENGTH=" + headers["Content-Length"];
	_env.push_back(content_length);
	_env.push_back(content_type);

	// LOGIQUE PHP
	if (real_path.size() < 4)
		return ;
	std::string extension = real_path.substr(real_path.size() - 4);
	if (extension != ".php")
		return ;
	_isphp = true ;
	_env.push_back("REDIRECT_STATUS=200");
	_env.push_back(std::string("SCRIPT_FILENAME=") + ABSOLUTE_PATH + "website/api/hello.php");
}

void	Response::cgi()
{
	_isphp = false ;
	if (!isValidMethodExtension() || !isValidCgi())
	{
		badRequest();
		return ;
	}
	create_env();
	std::vector<char*> envp;
	for (size_t i = 0; i < _env.size(); ++i) {
		envp.push_back(const_cast<char*>(_env[i].c_str()));
	}
	envp.push_back(NULL);

	size_t pos = path.find('?');
	if (pos == std::string::npos)
		pos = path.size();
	std::string cgi_path = path.substr(0, pos);


	if (_request->getMethodType() == "GET")
		getCGI(cgi_path, envp);
	else if (_request->getMethodType() == "POST")
		postCGI(cgi_path, envp);
	else
	{
		_status = 405;
		badRequest();
	}
}


void	Response::postCGI(std::string cgi_path, std::vector<char *> envp)
{
	std::string body = _request->getBody();

	int	fd_stdin[2];
	int	fd_stdout[2];
	if (pipe(fd_stdin) == -1 || pipe(fd_stdout) == -1)
	{
		_status = 500;
		badRequest();
		std::cerr << "Problem with pipe." << std::endl;
		return ;
	}

	pid_t pid = fork();
	if (pid == -1) {
		_status = 500;
		badRequest();
		std::cerr << "Problem with fork" << std::endl;
		return ;
	}
	if (pid == 0) // <= enfant
	{
		close(fd_stdin[1]);
		close(fd_stdout[0]);
		dup2(fd_stdin[0], STDIN_FILENO); // on clone le STDIN sur lequel on va lire dessus
		dup2(fd_stdout[1], STDOUT_FILENO);
		close(fd_stdin[0]);
		close(fd_stdout[1]);

		char *av[3];
		if (_isphp)
		{
			av[0] = const_cast<char*>(PHP_PATH);
			av[1] = const_cast<char *>(cgi_path.c_str());
			av[2] = NULL;
		}
		else
		{
			av[0] = const_cast<char *>(cgi_path.c_str());
			av[1] = NULL;
			av[2] = NULL;
		}
		execve(av[0], av, envp.data());

		std::cout << "CGI execution failed" << std::endl; // Que doit on faire ?
		exit(1);
	}
	else // <= parent
	{
		size_t nb_written = 0;
		size_t total = 0;
		while (total < body.size())
		{
			nb_written = write(fd_stdin[1], body.data() + total, body.size() - total);
			if (nb_written <= 0)
				break;
			total += nb_written;
		}
		close(fd_stdin[1]);
		close(fd_stdout[1]);

		char buffer[4096];
		std::string cgi_body;
		ssize_t nb_read = 1;
		while (nb_read) {
			nb_read = read(fd_stdout[0], buffer, sizeof(buffer));
			cgi_body.append(buffer, nb_read);
		}
		close(fd_stdout[0]);


		int status;
		waitpid(pid, &status, 0);
		_status = 200;
		parseCgiBody(cgi_body);
		_headers["Content-Length"] = intToStdString(_body.size());
	}
}
