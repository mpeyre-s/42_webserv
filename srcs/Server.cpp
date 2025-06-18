#include "../includes/Server.hpp"

Location::Location(std::vector<std::string> confFile) {
	for (size_t i = 0; i < confFile.size(); i++) {
		std::vector<std::string> line_token = split(confFile[i], " ");
		if (line_token.size() < 2)
			throw std::invalid_argument(confFile[i]);

		// root
		if (line_token[0] == "root") {
			std::string last_token = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (root.empty() && last_token.length() > 1 && last_token[last_token.length() - 1] == '/') {
				root = last_token;
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// index
		if (line_token[0] == "index") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (index.empty()) {
				index = line_token[1];
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// upload_dir
		if (line_token[0] == "upload_dir") {
			std::string last_token = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (upload_dir.empty() && last_token.length() > 1 && last_token[last_token.length() - 1] == '/') {
				upload_dir = last_token;
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// client_max_body_size
		if (line_token[0] == "client_max_body_size") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (!client_max_body_size) {
				int nb = atoi(line_token[1].c_str());
				if (nb <= MAX_BODY_SIZE && nb > 0)
					client_max_body_size = nb * (1024 * 1024);
				else
					throw std::invalid_argument(confFile[i]);
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// auto_index
		if (line_token[0] == "auto_index") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (!auto_index) {
				if (line_token[1] == "on")
					auto_index = true;
				else
					auto_index = false;
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// allow_methods
		if (line_token[0] == "allow_methods") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (allowed_methods.size() == 0 && (line_token.size() > 1 && line_token.size() < 5)) {
				int get_flag = 0;
				int post_flag = 0;
				int delete_flag = 0;
				for (size_t j = 1; j < line_token.size(); j++) {
					if (line_token[j] == "GET")
						get_flag++;
					else if (line_token[j] == "POST")
						post_flag++;
					else if (line_token[j] == "DELETE")
						delete_flag++;
					else
						throw std::invalid_argument(confFile[i]);
					allowed_methods.push_back(line_token[j]);
				}
				if (get_flag > 1 || post_flag > 1 || delete_flag > 1)
					throw std::invalid_argument(confFile[i]);
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// error_pages
		if (line_token[0] == "error_page") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (line_token.size() ==  3) {
				int error_code = atoi(line_token[1].c_str());
				if (error_code < 100 || error_code > 511)
					throw std::invalid_argument(confFile[i]);
				std::ifstream file(line_token[2].c_str());
				if (!file.is_open())
					throw std::invalid_argument(confFile[i]);
				if (checkFileExtension(line_token[2], ".html") == false)
					throw std::invalid_argument(confFile[i]);
				if (error_pages.find(error_code) != error_pages.end())
					throw std::invalid_argument(confFile[i]);
				error_pages[error_code] = line_token[2];
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// cgi_extensions
		if (line_token[0] == "cgi") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (cgi_extensions.size() == 0 && line_token.size() > 1) {
				for (size_t j = 1; j < line_token.size(); j++) {
					if (line_token[j] == ".php" || line_token[j] == ".py")
						cgi_extensions.push_back(line_token[j]);
					else
						throw std::invalid_argument(confFile[i]);
				}
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// cgi_path
		if (line_token[0] == "cgi_path") {
			std::string last_token = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (cgi_path.empty() && last_token.length() > 1 && last_token[last_token.length() - 1] == '/') {
				cgi_path = line_token[1];
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// redirect_code
		if (line_token[0] == "return") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (!redirect_code && redirect_url.empty() == true) {
				int nb = atoi(line_token[1].c_str());
				if (nb > 300 && nb < 399)
					redirect_code = nb;
				else
					throw std::invalid_argument(confFile[i]);

				redirect_url = line_token[2];
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}
	}
}

Location::~Location() {}

Server::Server(std::vector<std::string> confFile) : _default(false) {
	// ==================== Default Server parsing ==========================
	bool reach_end_default = false;
	size_t actual_line = 0;
	for (size_t i = 0; i < confFile.size() && reach_end_default == false; i++) {
		++actual_line;

		if (i + 1 < confFile.size() && confFile[i + 1].find("location") != std::string::npos)
			reach_end_default = true;

		std::vector<std::string> line_token = split(confFile[i], " ");
		if (line_token.size() < 2)
			throw std::invalid_argument(confFile[i]);

		// server name
		if (line_token[0] == "server_name") {
			std::string last_token = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (server_name.empty()) {
				server_name = last_token;
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// listen
		if (line_token[0] == "listen") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (host.empty() && !port && validIpFormat(line_token[1])) {
				size_t two_points = line_token[1].find(":");
				host = line_token[1].substr(0, two_points);
				port = atoi(line_token[1].substr(two_points + 1).c_str());
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// root
		if (line_token[0] == "root") {
			std::string last_token = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (root.empty() && last_token.length() > 1 && last_token[last_token.length() - 1] == '/') {
				root = last_token;
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// index
		if (line_token[0] == "index") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (index.empty()) {
				index = line_token[1];
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// client_max_body_size
		if (line_token[0] == "client_max_body_size") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (!client_max_body_size) {
				int nb = atoi(line_token[1].c_str());
				if (nb <= MAX_BODY_SIZE && nb > 0)
					client_max_body_size = nb * (1024 * 1024);
				else
					throw std::invalid_argument(confFile[i]);
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// auto_index
		if (line_token[0] == "auto_index") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (!auto_index) {
				if (line_token[1] == "on")
					auto_index = true;
				else
					auto_index = false;
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// allow_methods
		if (line_token[0] == "allow_methods") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (allowed_methods.size() == 0 && (line_token.size() > 1 && line_token.size() < 5)) {
				int get_flag = 0;
				int post_flag = 0;
				int delete_flag = 0;
				for (size_t j = 1; j < line_token.size(); j++) {
					if (line_token[j] == "GET")
						get_flag++;
					else if (line_token[j] == "POST")
						post_flag++;
					else if (line_token[j] == "DELETE")
						delete_flag++;
					else
						throw std::invalid_argument(confFile[i]);
					allowed_methods.push_back(line_token[j]);
				}
				if (get_flag > 1 || post_flag > 1 || delete_flag > 1)
					throw std::invalid_argument(confFile[i]);
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}

		// error_pages
		if (line_token[0] == "error_page") {
			line_token[line_token.size() - 1] = line_token[line_token.size() - 1].substr(0, line_token[line_token.size() - 1].length() - 1);
			if (line_token.size() ==  3) {
				int error_code = atoi(line_token[1].c_str());
				if (error_code < 100 || error_code > 511)
					throw std::invalid_argument(confFile[i]);
				std::ifstream file(line_token[2].c_str());
				if (!file.is_open())
					throw std::invalid_argument(confFile[i]);
				if (checkFileExtension(line_token[2], ".html") == false)
					throw std::invalid_argument(confFile[i]);
				if (error_pages.find(error_code) != error_pages.end())
					throw std::invalid_argument(confFile[i]);
				error_pages[error_code] = line_token[2];
				continue;
			} else
				throw std::invalid_argument(confFile[i]);
		}


	}

	// if no location return to avoid parse location
	bool seen = false;
	for (size_t i = actual_line; i < confFile.size(); i++) {
		if (confFile[i].find("location") != std::string::npos) {
			seen = true;
			break;
		}
	}
	if (seen == false)
		return;

	std::cout << "LOCATION PARSING BEGIN" << std::endl;

	// ==================== Locations parsing ==========================
	bool in_location_block = false;
	std::vector<std::string> locationBlock;
	std::string location_name;
	for (size_t i = actual_line; i < confFile.size(); i++) {
		std::string line = confFile[i];
		std::vector<std::string> line_token = split(line, " ");

		if (!in_location_block) {
			if (line_token.size() >= 3 && line_token[0] == "location" && line_token[1][0] == '/' && line.find('{') != std::string::npos) {
				location_name = line_token[1];
				in_location_block = true;
				locationBlock.clear();
				continue;
			}
			else if (line_token[0] == "location") {
				throw std::invalid_argument(confFile[i]);
			}
		} else {
			if (line.find('}') != std::string::npos) {
				try {
					locations[location_name] = new Location(locationBlock);
				} catch (const std::exception& e) {
					throw std::invalid_argument(e.what());
				}
				in_location_block = false;
				locationBlock.clear();
			} else
				locationBlock.push_back(line);
		}
	}
}

Server::~Server() {}

std::string Server::getServerName() const {
	return server_name;
}

std::string Server::getHost() const {
	return host;
}

int Server::getPort() const {
	return port;
}

std::string Server::getRoot() const {
	return root;
}

std::string Server::getIndex() const {
	return index;
}

const std::vector<std::string>& Server::getAllowedMethods() const {
	return allowed_methods;
}

const std::map<std::string, Location*>& Server::getLocations() const {
	return locations;
}

int Server::getMaxBodySize() const {
	return client_max_body_size;
}

bool Server::getAutoIndex() const {
	return auto_index;
}

std::map<int, std::string> Server::getErrorPages() const {
	return error_pages;
}

bool Server::getDefaultStatus() {
	return _default;
}

void Server::setServerName(const std::string& server) {
	server_name = server;
}
void Server::setHost(std::string& hostname){
	host = hostname;
}
void Server::setPort(int& porttmp){
	port = porttmp;
}
void Server::setRoot(std::string& rootname){
	root = rootname;
}
void Server::setIndex(std::string& indexname){
	index = indexname;
}

void Server::setLocation(const std::string path, Location *newLocation){
	locations[path] = newLocation;
}

void Server::addErrorPage(int err, std::string mess)
{
	error_pages[err] = mess;
}
void Server::addAllowedMethod(std::string method){
	allowed_methods.push_back(method);
}

void Server::setClientMaxBody(int clientMaxBody){
	client_max_body_size =  clientMaxBody;
}

void Server::setDefaultServer() {
	_default = true;
}
