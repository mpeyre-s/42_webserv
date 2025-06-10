#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Server.hpp"

class ConfigParsing {
private:
	std::vector<Server*> 					_list_servers;
	// std::vector<std::string>				_serverBlock;
	std::vector<std::vector<std::string> >	_serverList;

public:
	ConfigParsing(std::string &configFile);
	~ConfigParsing();

	std::vector<Server*> createServerList();
};
