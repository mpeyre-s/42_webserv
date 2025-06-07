#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "Server.hpp"

class ConfigParsing {
private:
	std::vector<Server*> _list_servers;

public:
	ConfigParsing(std::string &configFile);
	~ConfigParsing();

	std::vector<Server*> createServerList();
};
