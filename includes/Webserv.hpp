#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <map>
#include "Server.hpp"
#include "ClientConnexion.hpp"

#define TIMEOUT 1000 // le delai pour envoyer une requete

/*
struct pollfd {
    int   fd;         // descripteur de fichier à surveiller
    short events;     // événements demandés (masque de bits)
    short revents;    // événements qui se sont produits (masque de bits)
};
*/

struct FdInfo
{
	int			fd;
	sockaddr_in	addr; // bind struct
};


class Webserv {
	private:
		std::vector<Server*>	_list_servers;
		std::map<int, Server *> _correspondingServ;
		std::vector<FdInfo>		_tempFds;
		std::vector<pollfd>		_fds;
		std::map<int, ClientConnexion *> _clients; // int = fd client

		// fd
		void	prepareSockets();
		void	prepareFd();

		// utils
		bool	isClients(int i);

		//Server ~ Client
		void	handleNewConnexion(int);

		//Client
		void	handleClientReading(int i);
		void	handleClientWriting(int i);
		void	removeClientIfPossible(int i);

		// poll
		void	positivPoll();
		void	negativPoll(); // TODO
		void	timeoutPoll(); // TODO



	public:
		Webserv(std::vector<Server*> list_servers);
		~Webserv();

		void run();
};
