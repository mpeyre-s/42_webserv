#pragma once

#include <string>
#include <iostream>
#include "Server.hpp"

class ClientConnexion
{
	private:
		int			_fd;
		Server		*_server;
		std::string	bufferIn;
		std::string	bufferOut;

	public:
		ClientConnexion(int, Server *);
		~ClientConnexion();
};
