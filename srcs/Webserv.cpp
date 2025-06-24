#include "../includes/Webserv.hpp"

Webserv::Webserv(std::vector<Server*> list_servers) : _list_servers(list_servers) {
	for (size_t i = 0; i < _list_servers.size(); i++) {
		bool ip_already_in_struct = false;
		size_t j = 0;
		for (; j < _server_group.size(); j++) {
			if (_server_group[j].ip == _list_servers[i]->getHost() && _server_group[j].port == _list_servers[i]->getPort()) {
				ip_already_in_struct = true;
				break;
			}
		}
		if (ip_already_in_struct) {
			_server_group[j].list_server.push_back(_list_servers[i]);
			_server_group[j].nb_server++;
		}
		else {
			serverGroup new_group;
			new_group.ip = _list_servers[i]->getHost();
			new_group.port = _list_servers[i]->getPort();
			new_group.nb_server = 1;
			new_group.list_server.push_back(_list_servers[i]);
			new_group.fd = 0;
			_server_group.push_back(new_group);
		}
	}

	std::cout << "\033[32m[INFO] Server is ready to run\033[0m" << std::endl;
}

Webserv::~Webserv() {
	std::cout << "\033[32m[INFO] Server is killed\033[0m" << std::endl;

	// Fermer fds
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
		close(it->fd);
	}

	// Supprimer tous les clients
	for (std::map<int, ClientConnexion*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	_clients.clear();

	// clear les maps / vecteurs attention a bien delete les servers
	_list_servers.clear();
	_correspondingServ.clear();
	_tempFds.clear();
	_fds.clear();
	for (size_t i = 0; i < _server_group.size(); i++)
	{
		if (!_server_group[i].list_server.empty())
			_server_group[i].list_server.clear();
	}
	_server_group.clear();
}

// Cette fonction sert à créer et set-up les fds correspondant à chaque virtual host
void Webserv::prepareSockets()
{
	for (size_t i = 0; i < _server_group.size(); ++i)
	{
		struct FdInfo ep;

		// Création du socket
		ep.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (ep.fd < 0)
			throw std::runtime_error("Erreur lors de la création du socket");

		_server_group[i].fd = ep.fd;

		// Fctnl pour que les sockets soient non bloquantes
		fcntl(ep.fd, F_SETFL, O_NONBLOCK);

		// setsockopt pour réutiliser l'adresse
		int opt = 1;
		if (setsockopt(ep.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Erreur setsockopt()");

		// Initialisation de sockaddr_in pour Bind
		std::memset(&ep.addr, 0, sizeof(ep.addr));
		ep.addr.sin_family = AF_INET;
		ep.addr.sin_port = htons(_server_group[i].port);


	std::string host = _server_group[i].ip;

	if (host.empty() || host == "0.0.0.0") {
		ep.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		int ret = inet_pton(AF_INET, host.c_str(), &ep.addr.sin_addr);
		if (ret <= 0)
			throw std::runtime_error("Adresse IP invalide pour le host : " + host);
	}

	// Tentative de bind
	if (bind(ep.fd, reinterpret_cast<sockaddr*>(&ep.addr), sizeof(ep.addr)) < 0) {
		std::cerr << "Erreur bind sur IP " << host << ": " << strerror(errno) << std::endl;
		throw std::runtime_error("Erreur lors du bind");
	}
	// Listen
		if (listen(ep.fd, SOMAXCONN) < 0)
			throw std::runtime_error("Erreur listen");

		_tempFds.push_back(ep);

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

void	Webserv::changeEvents(int fd, short POLL)
{
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		if (_fds[i].fd == fd)
		{
			_fds[i].events = POLL;
			break;
		}
	}
}

bool Webserv::isClients(int fd) {
	std::map<int, ClientConnexion*>::iterator it = _clients.find(fd);
		return (it != _clients.end());
}

void Webserv::handleNewConnexion(int server_fd)
{
	// Gestion d'un nouvel fd client
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
	if (client_fd < 0) {
		return ;
	}

	struct pollfd	client_poll;
	client_poll.fd = client_fd;
	client_poll.events = POLLIN;
	client_poll.revents = 0;
	_fds.push_back(client_poll);

	// Création de l'instance de la class ClientConnexion pour gérer ce nouveau client
	// il va falloir envoyer la liste de serveur car on va devoir gerer le bon serveur correspondant
	ClientConnexion *client = new ClientConnexion(client_fd, _correspondingServ[server_fd], TO_READ); // ======= on a pas un pb ici ?
	_clients[client_fd] = client;
}

void	Webserv::handleClientReading(int fd)
{
	ClientConnexion *client = _clients[fd];
	char			buf[BUFFER_SIZE];

	if (client->hasTimedOut() == true)
	{
		removeClient(fd);
		return ;
	}

	int bytes = read(fd, buf, BUFFER_SIZE);
	if (bytes > 0)
	{
		client->UpdateActivity();
		client->appendToBuffer(buf, bytes);
		if (client->getState() == DONE_READING)
		{
			Request *request = new Request(client->getBufferIn(), client->getVecChar(), _server_group);
			Response* response = request->process(client->getServer());
			client->setBufferOut(response->getStringResponse()); // ce n'est pas l'upload qu'on renvoie, c'est la rep ok
			client->setKeepAlive(request->isKeepAlive());

			//std::cout << "===== BUFFER OUT =====" << std::endl << response->getStringResponse() << std::endl;

			delete request;
			delete response;

			client->setState(TO_WRITE);
			changeEvents(fd, POLLOUT);
		}
	}
	else if (bytes == 0)
		removeClient(fd);
	else
	{
		// Rien à lire pour l’instant : on attendra un autre POLLIN (ne devrait jamais arriver)
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		else
		{
			removeClient(fd);
		}
	}
}

void	Webserv::handleClientWriting(int fd)
{
	ClientConnexion *client = _clients[fd];
	std::string &buffer = client->getBufferOut();

	if (client->hasTimedOut() == true)
	{
		removeClient(fd);
		return ;
	}

	int bytesSent = send(fd, buffer.c_str(), buffer.size(), 0);
	if (bytesSent > 0)
	{
		client->UpdateActivity();
		client->removeFromBuffer(bytesSent);
		if (client->getState() == DONE_WRITING)
		{
			// faut il gerer autre chose ?
			client->clearBuffer();
			if (client->getKeep_alive() == false)
				removeClient(fd);
			else
				changeEvents(fd, POLLIN);
		}
	}
	else if (bytesSent == 0)
		removeClient(fd);
	else
	{
		// Rien à écrire pour l’instant : on attendra un autre POLLIN (ne devrait jamais arriver)
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		else
		{
			removeClient(fd);
		}
	}
}

void Webserv::removeClient(int fd)
{
	// Supprimer du vecteur _fds
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			break;
		}
	}

	// Supprimer et libérer le client
	std::map<int, ClientConnexion*>::iterator client_it = _clients.find(fd);
	if (client_it != _clients.end())
	{
		delete client_it->second;
		_clients.erase(client_it);
	}

	// Fermer le fd en dernier
	close(fd);
}


void Webserv::positivPoll()
{
	for (int i = _fds.size() - 1; i >= 0; --i)
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
					removeClient(_fds[i].fd);
			}
		}
	}
}

void Webserv::timeoutPoll()
{
	std::map<int, ClientConnexion *>::iterator it;
	it = _clients.begin();
	while (it != _clients.end())
	{
		int fd = it->first;
		ClientConnexion *client = _clients[fd];
		if (client->hasTimedOut())
		{
			removeClient(fd);
			it = _clients.upper_bound(fd);
		}
		else
			++it;
	}
}

void Webserv::negativPoll()
{
	std::cerr << "\033[1;31m[Error] poll() failed:\033[1m" << strerror(errno) << "\033[0m" << std::endl;

	exit(EXIT_FAILURE);
}


void Webserv::run()
{
	std::cout << "\033[32m[INFO] Server is running\033[0m" <<std::endl;

	prepareSockets();
	prepareFd();
	initCorrespondingServ();

	while (!g_stop)
	{
		int res = poll(_fds.data(), _fds.size(), TIMEOUT);
		if (res < 0)
		{
			if (errno == EINTR)
				continue;
			else
				negativPoll();
		}
		else if (res == TIMEOUT)
			timeoutPoll();
		else
			positivPoll();
	}
}

void Webserv::initCorrespondingServ() {
	for (size_t i = 0; i < _server_group.size(); i++) {
		_correspondingServ[_server_group[i].fd] = _server_group[i].list_server[0];

	}
}
