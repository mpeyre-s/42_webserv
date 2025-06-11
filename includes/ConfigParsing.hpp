#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Server.hpp"

class ConfigParsing {
private:
	std::vector<Server*> 					_list_servers;
	std::vector<std::vector<std::string> >	_serverBlocks;
	std::vector<std::vector<std::string> >	_tokenizedBlock;

public:
	ConfigParsing(std::string &configFile);
	~ConfigParsing();

	void 					parseServerBlock(const std::vector<std::string>& block, Server* server);
	std::vector<std::string> tokenizeLine(const std::string& line);
	std::vector<Server*>	createServerList();
};
