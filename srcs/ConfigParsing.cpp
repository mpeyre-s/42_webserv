#include "../includes/ConfigParsing.hpp"

ConfigParsing::ConfigParsing(std::string &configFile) {
	(void)configFile;
	_list_servers.push_back(new Server());
}

ConfigParsing::~ConfigParsing() {}

std::vector<Server*> ConfigParsing::createServerList() {
	return _list_servers;
}
