#include "../includes/ConfigParsing.hpp"

ConfigParsing::ConfigParsing(std::string &configFile) {
	(void)configFile;
	_list_servers.push_back(new Server());
}

ConfigParsing::~ConfigParsing() {}

std::vector<Server*> ConfigParsing::createServerList() {
	std::cout << "\033[32m[INFO] Configuration file parsed succesfuly\033[0m" << std::endl;
	return _list_servers;
}
