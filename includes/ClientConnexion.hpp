#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"


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

		//Response	*_response;
		Request		*_request;

		bool	isDoneReading();
		bool	checkChunked(size_t);
		bool	checkContentLength(size_t, size_t);
	public:
		ClientConnexion(int, Server *, State);
		~ClientConnexion(); // TODO

		void	appendToBuffer(char *, int);
		State	getState();
		std::string &getBufferIn();
		void	setBufferOut(std::string buff);
		Server *getServer();
		void	setState(State);
		void	setRequest(Request *);
};
