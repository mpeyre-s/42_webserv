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
	bool inServerBlock = false;
	std::vector<std::string> serverBlock;
	while (getline(cfFile, line))
	{
		if (line.find("server {") != std::string::npos)
			inServerBlock = true;
		if (line.find('{') != std::string::npos)
			bracket++;
		if (line.find('}') != std::string::npos)
			bracket--;
		if (inServerBlock)
			serverBlock.push_back(line);
		if (bracket == 0 && inServerBlock)
		{
			_serverBlocks.push_back(serverBlock);
			serverBlock.clear();
			inServerBlock = false;
		}
	}
	// DEBUG AFFICHAGE SERVEURS PAR BLOC
	// for(std::vector<std::vector<std::string> >::const_iterator it1 = _serverBlocks.begin(); it1 != _serverBlocks.end(); it1++)
	// {
	// 	std::cout << "------SERVER------" << std::endl;
	// 	for (std::vector<std::string>::const_iterator it2 = it1->begin(); it2 != it1->end(); it2++)
	// 		std::cout << *it2 << std::endl; 
	// }
}

ConfigParsing::~ConfigParsing() {}

std::vector<std::string> ConfigParsing::tokenizeLine(const std::string& line)
{
	std::vector<std::string> tokens;
	std::string currentToken;

	std::string lineTmp = line;
	size_t commentPos = line.find('#');
	if (commentPos != std::string::npos)
		lineTmp = line.substr(0, commentPos);
	for(size_t i = 0; i < lineTmp.length(); i++)
	{
		char c = lineTmp[i];
		if (std::isspace(c))
		{
			if(!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else if (c == ';' || c == ':')
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

Location setLocation(std::vector<std::vector<std::string> > LocationTMP, std::string path)
{
	Location locTmp;

	locTmp.path = path;
	locTmp.client_max_body_size = 0;
	for(size_t i = 0; i < LocationTMP.size(); i++)
	{
		std::vector<std::string> loc = LocationTMP[i];
		if (loc[0] == "allow_methods")
		{
			for (size_t j = 0; j < loc.size(); j++)
				if ((loc[j] == "GET" || loc[j] == "POST" || loc[j] == "DELETE"))
					locTmp.allowed_methods.push_back(loc[j]);
		}
		if (loc[0] == "root")
			if (loc.size() > 1 && !loc[1].empty())
				locTmp.root = loc[1];
		if (loc[0] == "index")
			if (loc.size() > 1 && !loc[1].empty())
				locTmp.index = loc[1];
		if (loc[0] == "upload_dir")
			if (loc.size() > 1 && !loc[1].empty())
				locTmp.upload_dir = loc[1];
		if (loc[0] == "autoindex")
		{
			if (loc.size() > 1 && !loc[1].empty() && loc[1] == "on")
				locTmp.auto_index = true;
			else if (loc.size() > 1 && !loc[1].empty() && loc[1] == "off")
				locTmp.auto_index = false;
			else
				std::cerr << "Bad autoindex in config file" << std::endl;
		}
		if (loc[0] == "client_max_body_size")
		{
			if (loc.size() > 1 && !loc[1].empty())
			{
				std::stringstream ss(loc[1]);
				ss >> locTmp.client_max_body_size;
			}
		}
		if (loc[0] == "cgi" && loc[0].length() == 3)
		{
			if (loc.size() > 1)
				for (size_t j = 1; j < loc.size(); j++)
					locTmp.cgi_extensions.push_back(loc[j]);
		}
		if (loc[0] == "cgi_path")
			if (loc.size() > 1 && !loc[1].empty())
				locTmp.cgi_path = loc[1];
		if (loc[0] == "return")
		{
			if (loc.size() > 1 && !loc[1].empty())
			{
				std::stringstream ss(loc[1]);
				ss >> locTmp.redirect_code;
			}
			if (loc.size() > 2 && !loc[2].empty())
				locTmp.redirect_url = loc[2];
		}
		if (loc[0] == "error_page")
		{
			if (loc.size() > 2 && !loc[1].empty() && !loc[2].empty())
			{
				int error;
				std::stringstream ss(loc[1]);
				ss >> error;
				std::string err_path = loc[2];
				locTmp.error_pages[error] = err_path;
			}
		}
	}
	return locTmp;
}

void ConfigParsing::parseServerBlock(std::vector<std::vector<std::string> >	tokenizedBlock, Server* server)
{
	server->setClientMaxBody(0);
	std::vector<std::vector<std::string> > LocationTMP;
	for (size_t i = 0; i < tokenizedBlock.size(); i++)
	{
		std::vector<std::string> line = tokenizedBlock[i];
		if(line[0] == "location")
		{
			Location loc;
			int braket = 0;
			std::string path;
			if (!line[1].empty())
				path = line[1];
			for (size_t j = 0; j < line.size(); j++)
			{
				if (line[j].find('{') != std::string::npos)
					braket++;
				if (line[j].find('}') != std::string::npos)
					braket--;
			}
			while (braket != 0 && ++i < tokenizedBlock.size())
			{
				int closeBraket = 0;
				for (size_t j = 0; j < tokenizedBlock[i].size(); j++)
				{
					if(tokenizedBlock[i][j].find('}') != std::string::npos)
						closeBraket++;
				}
				if (braket - closeBraket == 0)
					break;
				LocationTMP.push_back(tokenizedBlock[i]);
				for (size_t j = 0; j < tokenizedBlock[i].size(); j++)
				{
					if(tokenizedBlock[i][j].find('{') != std::string::npos)
						braket++;
					if(tokenizedBlock[i][j].find('}') != std::string::npos)
						braket--;
				}
			}
			loc = setLocation(LocationTMP, path);
			LocationTMP.clear();
			server->setLocation(path, loc);
		}
		if (line[0] == "server_name")
			if (line.size() > 1 && !line[1].empty()) 
				server->setServerName(line[1]);
		if (line[0] == "listen")
		{
			if (line.size() > 1 && !line[1].empty()) 
				server->setHost(line[1]);
			if (line.size() > 2 && !line[2].empty())
			{
				int port;
				std::stringstream ss(line[2]);
				ss >> port;
				server->setPort(port);
			}
		}
		if (line[0] == "root")
			if (line.size() > 1 && !line[1].empty()) 
				server->setRoot(line[1]);
		if (line[0] == "index")
			if (line.size() > 1 && !line[1].empty()) 
				server->setIndex(line[1]);
		if (line[0] == "error_page")
		{
			if (line.size() > 2 && !line[1].empty() && !line[2].empty())
			{
				int error;
				std::stringstream ss(line[1]);
				ss >> error;
				std::string err_path = line[2];
				server->addErrorPage(error, err_path);
			}
		}
		if (line[0] == "client_max_body_size")
		{
			if (line.size() > 1 && !line[1].empty())
			{
				int clienttmp;
				std::stringstream ss(line[1]);
				ss >> clienttmp;
				server->setClientMaxBody(clienttmp);
			}
		}
		if (line[0] == "allow_methods")
		{
			for (size_t j = 0; j < line.size(); j++)
				if ((line[j] == "GET" || line[j] == "POST" || line[j] == "DELETE"))
					server->addAllowedMethod(line[j]);
					
		}
	}
}


void ConfigParsing::tokenizeServerBlock(const std::vector<std::string>& block, Server* server)
{
	std::vector<std::string> token;
	std::vector<std::vector<std::string> >	tokenizedBlock;

	for(std::vector<std::string>::const_iterator it = block.begin(); it != block.end(); it++)
	{
		token = tokenizeLine(*it);
		if (!token.empty())
		{
			tokenizedBlock.push_back(token);
			token.clear();
		}
	}
	// //DEBUG : Affichage des tokens
	// int i = 0;
	// for(std::vector<std::vector<std::string> >::const_iterator it1 = tokenizedBlock.begin(); it1 != tokenizedBlock.end(); it1++)
	// {
	// 	i++;
	// 	std::cout << "------Tokenisation ligne " << i << " ------" << std::endl;
	// 	for (std::vector<std::string>::const_iterator it2 = it1->begin(); it2 != it1->end(); it2++)
	// 		std::cout << "[" << *it2 << "]" << " ";
	// 	std::cout << std::endl; 
	// }
	parseServerBlock(tokenizedBlock, server);
	tokenizedBlock.clear();
	std::cout << std::endl; 
}

void printServerTest(Server *server)
{
	std::string host = server->getServerName();
	std::map<std::string, Location> locations = server->getLocations();

	std::cout << "Server name = " << host << std::endl;

	for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		const Location& loc = it->second;
		std::cout << "Location path = " << it->first << std::endl;

		for (size_t i = 0; i < loc.allowed_methods.size(); ++i)
		{
			if (!loc.allowed_methods[i].empty())
			std::cout << "  Allowed methods: ";
			std::cout << loc.allowed_methods[i] << " " << std::endl;
		}
		std::cout << std::endl;
		if (!loc.root.empty())
			std::cout << "  Root: " << loc.root << std::endl;
		if (!loc.index.empty())
			std::cout << "  Index: " << loc.index << std::endl;
		if (!loc.upload_dir.empty())
			std::cout << "  Upload dir: " << loc.upload_dir << std::endl;
		std::cout << "  Autoindex: " << (loc.auto_index ? "on" : "off") << std::endl;

		if (loc.client_max_body_size != 0)
			std::cout << "  Client max body size: " << loc.client_max_body_size << std::endl;

		for (size_t i = 0; i < loc.cgi_extensions.size(); ++i)
		{
			if (!loc.cgi_extensions[i].empty())
			std::cout << "  CGI extensions: ";
			std::cout << loc.cgi_extensions[i] << " ";
		}
		std::cout << std::endl;

		if (!loc.cgi_path.empty())
		std::cout << "  CGI path: " << loc.cgi_path << std::endl;

		std::cout << "  Redirect: ";
		if (loc.redirect_code > 0 && !loc.redirect_url.empty())
			std::cout << loc.redirect_code << " -> " << loc.redirect_url << std::endl;
		else
			std::cout << "None" << std::endl;

		std::cout << "  Error pages:" << std::endl;
		for (std::map<int, std::string>::const_iterator ep = loc.error_pages.begin(); ep != loc.error_pages.end(); ++ep)
			std::cout << "    " << ep->first << " => " << ep->second << std::endl;

		std::cout << std::endl;
	}
}

bool ConfigParsing::checkConformity(Server* server)
{
	if (server->getHost() != " 0.0.0.0")
		return false;
	if (server->getPort() != 8080)
		return false;
	return true;

}

bool ConfigParsing::isValidIp(std::string ip)
{
	int isPoint;
	int pos;
	std::string valueStr
	int value;

	for (size_t i = 0; i < ip.length(); i++)
	{
		if (ip[i] == '.')
		{
			pos = i;
			valueStr = ip.substr(0, pos);

		}
	}
}
std::vector<Server*> ConfigParsing::createServerList() {
	std::cout << "\033[32m[INFO] Configuration file parsed succesfuly\033[0m" << std::endl;
	for (size_t i = 0; i < _serverBlocks.size(); i++)
	{
		Server* server = new Server;
		tokenizeServerBlock(_serverBlocks[i], server);
		printServerTest(server);
		_list_servers.push_back(server);
		if (!checkConformity(_list_servers[0]))
			throw ConfigParsingError();
	}
	return _list_servers;
}

const char* ConfigParsing::ConfigParsingError::what() const throw()
{
	return ("Error in parsing");
}
