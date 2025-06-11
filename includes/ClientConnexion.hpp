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
	TO_WRITE,
	WRITING,
	DONE_WRITING
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
		bool 	isDoneWriting();

		bool	checkChunked(size_t);
		bool	checkContentLength(size_t, size_t);

	public:
		ClientConnexion(int, Server *, State);
		~ClientConnexion(); // TODO

		void	appendToBuffer(char *, int);
		void	removeFromBuffer(int bytesSent);

		State		getState();
		Server		*getServer();
		std::string	&getBufferIn();
		std::string	&getBufferOut();

		void	setBufferOut(std::string buff);
		void	setState(State);
		void	setRequest(Request *);

		void	clearBuffer();
};
