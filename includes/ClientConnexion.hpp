#pragma once

#include <string>
#include <iostream>
#include "Server.hpp"


enum State
{
	TO_READ,
	READING,
	DONE_READING,
	TO_WRITE
};

class ClientConnexion
{
	private:
		int			_fd;
		Server		*_server;
		State		_state;
		std::string	bufferIn;
		std::string	bufferOut;
		// Il faut aussi faire un time ici

		bool	isDoneReading(); // TODO
	public:
		ClientConnexion(int, Server *, State);
		~ClientConnexion(); // TODO

		void	appendToBuffer(char *, int);
		State	getState();
		void	setState(State);
};
