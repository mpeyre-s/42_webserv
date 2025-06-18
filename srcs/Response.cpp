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
	unsuported_media_path = "resources/unsuported_media_path.html";
	forbidden_path = "resources/forbidden.html";

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
	if (cur_location.error_pages.find(415) != cur_location.error_pages.end())
		unsuported_media_path = cur_location.error_pages[415];
	if (cur_location.error_pages.find(403) != cur_location.error_pages.end())
		forbidden_path = cur_location.error_pages[403];

	std::cout << "The location is : " << cur_location.path << std::endl;
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
	else if (_status == 415) {
		_status = 413;
		_text_status = "Unsupported Media Type";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(unsuported_media_path));
		_body = pathfileToStringBackslashs(unsuported_media_path);
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
	_isCGI = false ;

	if (cur_location.cgi_path.empty() == false && cur_location.cgi_extensions.size() > 0) {
		path = cur_location.cgi_path +  _request->getPathToResource().substr(cur_location.path.length());
		_isCGI = true;
	}
	else {
		path = cur_location.root + _request->getPathToResource().substr(cur_location.path.length());
		_isCGI = true;
	}

	if (path[path.length() - 1] == '/' && cur_location.auto_index == false)
		path.append(cur_location.index);
	else if (_request->getPathToResource() == potential_server)
		path.append("/");

	// compose path with filename for delete method
	if (_request->getMethodType() == "DELETE") {
		size_t i = path.find("?");
		if (i != std::string::npos) {
			path = path.substr(0, i);
			path.append("/" + _request->getPathToResource().substr(_request->getPathToResource().find("=") + 1));
		}
	}
	if (_isCGI)
		return ;

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

// ======================================================= Toute la logique de get ===================================================================

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
			std::cout << "filename = " << _filename << std::endl;
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

void		Response::parseBodyBinary(std::vector<char> vec, size_t len)
{
	(void)len;
	std::string file_path = _server->getUploadDir() + "/" + _filename; // ça serait plus secure de faire une verification pour le "/" + file path already exist
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
		write(1, &vec[start], 1);
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
	// Je pense que cette logique n'est pas bonne
	std::string file_path = _server->getUploadDir() + "/" + _filename; // ça serait plus secure de faire une verification pour le "/"
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
				parseBodyBinary(_request->getVecChar(), _request->getBufferLen());
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
	setPath();
	if (!_correctPath)
		return ;
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

// ================================ DELETE LOGIC =================================

void	Response::Delete() {
	setPath();
	if (!_correctPath)
		return ;

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
		_text_status = "Forbidden";
		_headers["Content-Type"] = "text/html";
		_headers["Content-Length"] = intToStdString(getFileOctetsSize(forbidden_path));
		_body = pathfileToStringBackslashs(forbidden_path);
	}
}

void	Response::process()
{
	if (_badRequest)
		badRequest();
	if (isCGI()) // il faut mieux le gerer. Parfois c'est un cgi c'est juste qu'il n'est pas handle
		cgi(); // attention je ne gere pas bien les erreurs, il faut qu'à chaque fois je renvoie un code d'erreur ou explication
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

Response::~Response() {}










bool	Response::isCGI()
{
	setPath(); // <== me donne l'info avec _isCGI

	// Check Method available
	std::string Method = _request->getMethodType();
	std::vector<std::string>::iterator it = cur_location.allowed_methods.begin();

	for (; it != cur_location.allowed_methods.end(); ++it)
	{
		if (*it == Method)
			break ;
	}
	if (it == cur_location.allowed_methods.end())
		return false ;

	//Check extension available
	size_t pos = path.find('?');
	if (pos == std::string::npos)
		pos = path.size();
	std::string sub_path = path.substr(0, pos);
	std::cout << sub_path << std::endl;
	for (std::vector<std::string>::iterator it = cur_location.cgi_extensions.begin(); it != cur_location.cgi_extensions.end(); ++it)
	{
		size_t ext_pos = sub_path.find(*it);

		if (ext_pos != std::string::npos)
			return true ;
	}
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
		return false;
	}

	//Vérifier que le fichier est exécutable
	struct stat st;
	if (stat(check_path.c_str(), &st) == -1)
		return false;
	if (!(st.st_mode & S_IXUSR)) // Vérifie si le propriétaire peut exécuter le fichier
		return false;


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


void	Response::execCGI(std::string cgi_path, std::vector<char *> envp )
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
	if (!isValidCgi())
		return ;
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

	execCGI(cgi_path, envp);
}
