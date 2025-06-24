#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <map>
#include "Server.hpp"
#include "ClientConnexion.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "main.hpp" // <= double inclusion ?
#include "utils.hpp"

#define TIMEOUT -1 // le delai pour envoyer une requete
static const size_t BUFFER_SIZE = 2048;

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
	sockaddr_in	addr;
};



class Webserv {
	private:
		std::vector<Server*>	_list_servers;
		std::vector<serverGroup> _server_group;
		std::map<int, Server *> _correspondingServ;
		std::vector<FdInfo>		_tempFds;
		std::vector<pollfd>		_fds;
		std::map<int, ClientConnexion *> _clients; // int = fd client

		// fd
		void	prepareSockets();
		void	prepareFd();
		void	changeEvents(int, short);

		// utils
		bool	isClients(int i);

		//Server ~ Client
		void	handleNewConnexion(int);

		//Client
		void	handleClientReading(int i);
		void	handleClientWriting(int i);
		void	removeClient(int i);

		// poll
		void	positivPoll();
		void	negativPoll(); // TODO
		void	timeoutPoll();


		void initCorrespondingServ();

	public:
		Webserv(std::vector<Server*> list_servers);
		~Webserv(); // TODO

		void run();
};

