#include "../includes/ConfigParsing.hpp"

bool isConfigFileOpenable(std::string &path) {
	if (path.length() < 6)
		return false;

	if (checkFileExtension(path, ".conf") == false)
		return false;

	std::ifstream file(path.c_str());
	if (!file.is_open())
		return false;
	file.close();

	return true;
}

bool isAllCurlyBracesClosed(std::string &path) {
	size_t open_brace = 0;
	size_t closed_brace = 0;

	std::ifstream file(path.c_str());

	std::string line;
	while (std::getline(file, line)) {
		for (size_t i = 0; i < line.length(); ++i) {
			if (line[i] == '{')
				++open_brace;
			else if (line[i] == '}')
				++closed_brace;
		}
	}
	file.close();
	return open_brace == closed_brace;
}

size_t nbPotentialServers(std::string &path) {
	std::ifstream file(path.c_str());

	size_t nb_servers = 0;

	std::string line;
	while (std::getline(file, line)) {
		if (line.find("server {") != std::string::npos)
			nb_servers++;
	}
	file.close();
	return nb_servers;
}

int isAllLinesEndedBySemicolon(std::string &path) {
	std::ifstream file(path.c_str());
	std::string line;
	size_t nb_line = 0;
	while (std::getline(file, line)) {
		nb_line++;
		size_t start = line.find_first_not_of(" \t\r\n");
		size_t end = line.find_last_not_of(" \t\r\n");
		if (start == std::string::npos)
			continue;
		std::string trimmed = line.substr(start, end - start + 1);

		if (trimmed.find('{') != std::string::npos || trimmed.find('}') != std::string::npos)
			continue;

		if (trimmed.empty() || trimmed[trimmed.length() - 1] != ';')
			return nb_line;
	}
	file.close();
	return 0;
}

ConfigParsing::ConfigParsing(std::string &configFile) {
	// basics tests
	if (isConfigFileOpenable(configFile) == false)
		throw std::invalid_argument("Invalid configuration file: must be openable and have .conf extension");
	if (isAllCurlyBracesClosed(configFile) == false)
		throw std::invalid_argument("Invalid configuration file: not all curly braces closed");
	std::string error_semicolon_line = "Invalid configuration file: not all lines ended by semicolon. Line: ";
	if (isAllLinesEndedBySemicolon(configFile) != 0)
		throw std::invalid_argument(error_semicolon_line.append(intToStdString(isAllLinesEndedBySemicolon(configFile))));
	size_t nb_potential_servers = nbPotentialServers(configFile);
	if (nb_potential_servers < 1)
		throw std::invalid_argument("Invalid configuration file: must be minimum 1 server");

	// slice config file per server, trim leading spaces, skip empty lines
	std::ifstream file(configFile.c_str());
	std::string line;
	std::vector<std::string> serverBlock;

	bool in_server_block = false;
	int brace_count = 0;
	while (std::getline(file, line)) {
		size_t start = line.find_first_not_of(" \t\r\n");
		size_t end = line.find_last_not_of(" \t\r\n");
		std::string trimmed;
		if (start == std::string::npos)
			trimmed = "";
		else
			trimmed = line.substr(start, end - start + 1);

		if (trimmed.empty())
			continue;

		if (!in_server_block && trimmed.find("server") == 0 && trimmed.find("{") != std::string::npos) {
			in_server_block = true;
			brace_count = 0;
		}

		if (in_server_block) {
			serverBlock.push_back(trimmed);
			for (size_t i = 0; i < trimmed.length(); ++i) {
				if (trimmed[i] == '{')
					++brace_count;
				else if (trimmed[i] == '}')
					--brace_count;
			}
			if (brace_count == 0) {
				try {
					if (serverBlock.back() == "}")
						serverBlock.pop_back();

					_list_servers.push_back(new Server(serverBlock));
					serverBlock.clear();
				}
				catch (const std::exception& e) {
					throw std::invalid_argument(std::string("Parsing near line => ") + e.what());
				}
				in_server_block = false;
			}
		}
	}

	file.close();

	// logic tests (minimum required)
	if (_list_servers.size() < 1)
		throw std::invalid_argument("Parsing: Must be minimum 1 server");
	for (size_t i = 0; i < _list_servers.size(); i++) {
		if (_list_servers[i]->getHost().empty() || _list_servers[i]->getPort() == 0 || _list_servers[i]->getServerName().empty() || _list_servers[i]->getRoot().empty())
			throw std::invalid_argument("Parsing: Must be minimum by server: a server_name + valid listen (0.0.0.0:0000) + valid root");
		if (_list_servers[i]->getIndex().empty() && _list_servers[i]->getAutoIndex() == false)
			throw std::invalid_argument("Parsing: Must be an index if auto index off");
		if (_list_servers[i]->getLocations().size() == 0 && (_list_servers[i]->getAllowedMethods().size() == 0 || _list_servers[i]->getRoot().empty()))
			throw std::invalid_argument("Parsing: Must be minium 1 method allowed");
		if (_list_servers[i]->getLocations().size() == 0 && _list_servers[i]->getRoot().empty())
			throw std::invalid_argument("Parsing: Must be minium a root if not define in location");
	}

	// set default servers
	std::map<std::string, int> ip_seen;
	for (size_t i = 0; i < _list_servers.size(); i++) {
		std::string ip = _list_servers[i]->getHost();
		int port = _list_servers[i]->getPort();
		bool found = false;
		for (std::map<std::string, int>::iterator it = ip_seen.begin(); it != ip_seen.end(); ++it) {
			if (ip == it->first && port == it->second) {
				found = true;
				break;
			}
		}
		if (found == false) {
			ip_seen[ip] = port;
			_list_servers[i]->setDefaultServer();
		}
	}
}

ConfigParsing::~ConfigParsing() {
	for (size_t i = 0; i < _list_servers.size(); ++i) {
		delete _list_servers[i];
	}
	_list_servers.clear();
}

std::vector<Server*> ConfigParsing::createServerList() {
	std::cout << "\033[32m[INFO] Configuration file parsed succesfuly\033[0m" << std::endl;
	return _list_servers;
}
