#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Server.hpp"

typedef struct s_endpoint
{
	int socketFd;
	sockaddr_in addr;
} t_endpoint;

class Webserv {
	private:
		std::vector<Server*>	_list_servers;
		std::vector<t_endpoint>		_endpoints; //

		void createEndpoints();


	public:
		Webserv(std::vector<Server*> list_servers);
		~Webserv();

		void run();
};
