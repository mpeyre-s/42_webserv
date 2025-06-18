#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "Server.hpp"
#include "utils.hpp"

class ConfigParsing {
private:
	std::vector<Server*> _list_servers;

public:
	ConfigParsing(std::string &configFile);
	~ConfigParsing();

	std::vector<Server*> createServerList();
};
