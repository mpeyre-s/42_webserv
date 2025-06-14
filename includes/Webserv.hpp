#pragma once

#include <string>
#include <iostream>

#include "Server.hpp"

class Webserv {
private:
	std::vector<Server*> _list_servers;

public:
	Webserv(std::vector<Server*> list_servers);
	~Webserv();

	void run();
};
