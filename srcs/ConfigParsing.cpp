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
	if (nb_potential_servers < 2)
		throw std::invalid_argument("Invalid configuration file: must be minimum 2 servers");

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

	// check if the first serv is localhost and accepts all requests
	// check if serv has minimum required (for / if auto index off and no index throw error) etc...
	// ...
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
