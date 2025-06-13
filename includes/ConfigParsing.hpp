#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Server.hpp"

class ConfigParsing {
private:
	std::vector<Server*> 					_list_servers;
	std::vector<std::vector<std::string> >	_serverBlocks;
	//std::vector<std::vector<std::string> >	_tokenizedBlock;

public:
	ConfigParsing(std::string &configFile);
	~ConfigParsing();

	void 					tokenizeServerBlock(const std::vector<std::string>& block, Server* server);
	void 					parseServerBlock(std::vector<std::vector<std::string> >	tokenizedBlock, Server* server);
	std::vector<std::string> tokenizeLine(const std::string& line);
	std::vector<Server*>	createServerList();
	bool 					checkConformity(Server* server);
	bool 					isValidIp(std::string ip);

	class ConfigParsingError : public std::exception
	{
		const char* what() const throw();
	};

};
