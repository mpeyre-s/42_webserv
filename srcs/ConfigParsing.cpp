#include "../includes/ConfigParsing.hpp"

ConfigParsing::ConfigParsing(std::string &configFile) 
{
	std::ifstream cfFile(configFile.c_str());
	if (!cfFile)
	{
		std::cerr << "Cannot open config file";
		return;
	}
	std::string line;
	int bracket = 0;
	std::vector<std::string> serverBlock;
	while (getline(cfFile, line))
	{
		if (line.find('{') != std::string::npos)
			bracket++;
		if (line.find('}') != std::string::npos)
			bracket--;
		serverBlock.push_back(line);
		if (bracket == 0)
		{
			_serverBlocks.push_back(serverBlock);
			serverBlock.clear();
			//break;
		}
		std::cout << bracket << std::endl;
	}
	for(std::vector<std::vector<std::string> >::const_iterator it1 = _serverBlocks.begin(); it1 != _serverBlocks.end(); it1++)
	{
		std::cout << "------SERVER------" << std::endl;
		for (std::vector<std::string>::const_iterator it2 = it1->begin(); it2 != it1->end(); it2++)
			std::cout << *it2 << std::endl; 
	}
}

ConfigParsing::~ConfigParsing() {}

void ConfigParsing::parseServerBlock(const std::vector<std::string>& block, Server* server)
{
	std::vector<std::string> token;
	
	for(std::vector<std::string>::const_iterator it = block.begin(); it != block.end(); it++)
	{
		token = tokenizeLine(*it);
		if (!token.empty())
		{
			_tokenizedBlock.push_back(token);
			token.clear();
		}
	}
	for(std::vector<std::vector<std::string> >::const_iterator it1 = _tokenizedBlock.begin(); it1 != _tokenizedBlock.end(); it1++)
	{
		std::cout << "------Tokenisation------" << std::endl;
		for (std::vector<std::string>::const_iterator it2 = it1->begin(); it2 != it1->end(); it2++)
			std::cout << "[" << *it2 << "]" << " ";
		std::cout << std::endl; 
	}
	(void) server;
	_tokenizedBlock.clear();
}

std::vector<std::string> ConfigParsing::tokenizeLine(const std::string& line)
{
	std::vector<std::string> tokens;
	std::string currentToken;

	for(size_t i = 0; i < line.length(); i++)
	{
		char c = line[i];
		if (std::isspace(c))
		{
			if(!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else if (c == '{' || c == '}' || c == ';')
		{
			if(!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else currentToken += c;
	}
	if (!currentToken.empty())
		tokens.push_back(currentToken);
	return tokens;
}

std::vector<Server*> ConfigParsing::createServerList() {
	std::cout << "\033[32m[INFO] Configuration file parsed succesfuly\033[0m" << std::endl;
	for (size_t i = 0; i < _serverBlocks.size(); i++)
	{
		Server* server = NULL;
		parseServerBlock(_serverBlocks[i], server);
		//_list_servers.push_back(server);
	}
	return _list_servers;
}
