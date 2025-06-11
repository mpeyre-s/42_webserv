#pragma once

#include <string>
#include <iostream>
#include <sstream>
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
		int	_bodySize;
		// Il faut aussi faire un time ici TODO

		bool	isDoneReading();
		bool	checkChunked(size_t);
		bool	checkContentLength(size_t, size_t);
	public:
		ClientConnexion(int, Server *, State);
		~ClientConnexion(); // TODO

		void	appendToBuffer(char *, int);
		State	getState();
		void	setState(State);
};
