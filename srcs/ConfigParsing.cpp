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
	bool insideServerBlock;
	std::vector<std::string> serverBlock;
	while (getline(cfFile, line))
	{
		if (line.find("server {") != std::string::npos)
		{
			//bracket++;
			insideServerBlock = true;
		}
		if (line.find('{') != std::string::npos)
			bracket++;
		if (line.find('}') != std::string::npos)
			bracket--;
		serverBlock.push_back(line);
		if (bracket == 0)
		{
			_serverList.push_back(serverBlock);
			//serverBlock.clear();
			break;
		}
		std::cout << bracket << std::endl;
	}
	for(std::vector<std::string>::iterator it = serverBlock.begin(); it != serverBlock.end(); it++)
		std::cout << *it << std::endl;
	// for(std::vector<std::vector<std::string> >::iterator it1 = _serverList.begin(); it1 != _serverList.end(); it1++)
	// {

	// }

}

ConfigParsing::~ConfigParsing() {}

std::vector<Server*> ConfigParsing::createServerList() {
	std::cout << "\033[32m[INFO] Configuration file parsed succesfuly\033[0m" << std::endl;
	return _list_servers;
}
