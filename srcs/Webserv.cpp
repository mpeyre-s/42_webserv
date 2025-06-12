#include "../includes/Webserv.hpp"

Webserv::Webserv(std::vector<Server*> list_servers) : _list_servers(list_servers) {
	std::cout << "\033[32m[INFO] Server is ready to run\033[0m" << std::endl;
}

Webserv::~Webserv() {
	std::cout << "\033[32m[INFO] Server is killed\033[0m" << std::endl;
}

// Cette fonction sert à créer et set-up les fds correspondant à chaque virtual host
void Webserv::prepareSockets()
{
	for (size_t i = 0; i < _list_servers.size(); ++i)
	{
		struct FdInfo ep;

		// Création du socket
		ep.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (ep.fd < 0)
			throw std::runtime_error("Erreur lors de la création du socket");

		// Fctnl pour que les sockets soient non bloquantes
		fcntl(ep.fd, F_SETFL, O_NONBLOCK);

		// setsockopt pour réutiliser l'adresse
		int opt = 1;
		if (setsockopt(ep.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Erreur setsockopt()");

		// Initialisation de sockaddr_in pour Bind
		std::memset(&ep.addr, 0, sizeof(ep.addr));
		ep.addr.sin_family = AF_INET;
		ep.addr.sin_addr.s_addr = htonl(INADDR_ANY); // à vérifier -> dépend si le serveur à une IP
		ep.addr.sin_port = htons(_list_servers[i]->getPort());

		// Bind
		if (bind(ep.fd, reinterpret_cast<sockaddr*>(&ep.addr), sizeof(ep.addr)) < 0)
			throw std::runtime_error("Erreur lors du bind");

		// Listen
		if (listen(ep.fd, SOMAXCONN) < 0)
			throw std::runtime_error("Erreur listen");

		_tempFds.push_back(ep);
		_correspondingServ[ep.fd] = _list_servers[i];
	}
}


void	Webserv::prepareFd()
{
	for (size_t i = 0; i < _tempFds.size(); ++i)
	{
		struct pollfd pollFD;
		pollFD.fd = _tempFds[i].fd;
		pollFD.events = POLLIN;
		pollFD.revents = 0;

		_fds.push_back(pollFD);
	}
}

void Webserv::handleNewConnexion(int server_fd)
{
	// Gestion d'un nouvel fd client
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
	if (client_fd < 0) {
		std::cerr << "Error accept function" << std::endl;
		return ;
	}

	struct pollfd	client_poll;
	client_poll.fd = client_fd;
	client_poll.events = POLLIN;
	client_poll.revents = 0;
	_fds.push_back(client_poll);


	// Création de l'instance de la class ClientConnexion pour gérer ce nouveau client
	ClientConnexion *client = new ClientConnexion(client_fd, _correspondingServ[server_fd], TO_READ);
	_clients[client_fd] = client;
}

void	Webserv::handleClientReading(int fd)
{
	ClientConnexion *client = _clients[fd];
	char			buf[BUFFER_SIZE];

	int bytes = read(fd, buf, BUFFER_SIZE);
	if (bytes > 0)
	{
		client->appendToBuffer(buf, bytes);
		if (client->getState() == DONE_READING)
		{
			// LES PROBLEMES ICI MATHISSSSSSSSS => il faut appeler les class pour en finir avec ce client fidele
			Request *request = new Request(client->getBufferIn());
			Response* response = request->process(client->getServer());
			client->setBufferOut(response->getStringResponse());

			std::cout << "===== BUFFER OUT =====" << std::endl << response->getStringResponse() << std::endl;

			//client->setRequest(request);

			client->setState(TO_WRITE);
		}
	}
	else if (bytes == 0)
		removeClientIfPossible(fd);
	else
	{
		// Rien à lire pour l’instant : on attendra un autre POLLIN (ne devrait jamais arriver)
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		else
		{
			removeClientIfPossible(fd);
		}
	}

}

bool Webserv::isClients(int fd) {
	std::map<int, ClientConnexion*>::iterator it = _clients.find(fd);
		return (it != _clients.end());
}

void Webserv::positivPoll()
{
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		if (_fds[i].revents != 0)
		{
			if (isClients(_fds[i].fd) == false) // => serveur reçoit une co
			{
				if (_fds[i].revents & POLLIN)
					handleNewConnexion(_fds[i].fd);
			}
			else if (isClients(_fds[i].fd) == true) // => client veut faire un truc
			{
				if (_fds[i].revents & POLLIN)
					handleClientReading(_fds[i].fd);
				else if (_fds[i].revents & POLLOUT)
					handleClientWriting(_fds[i].fd);
				else if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
					removeClientIfPossible(_fds[i].fd);
			}
		}
	}
}

void Webserv::run()
{
	std::cout << "\033[32m[INFO] Server is running\033[0m" <<std::endl;

	prepareSockets();
	prepareFd();

	while (1)
	{
		int res = poll(_fds.data(), _fds.size(), TIMEOUT);
		if (res < 0)
			negativPoll();
		else if (res == TIMEOUT)
			timeoutPoll();
		else
			positivPoll();
	}
}


void Webserv::negativPoll() {
	return ;
}

void Webserv::timeoutPoll() {
	return ;
}

void	Webserv::handleClientWriting(int i) { (void)i; return ;}
void	Webserv::removeClientIfPossible(int i){ (void)i; return ;}
