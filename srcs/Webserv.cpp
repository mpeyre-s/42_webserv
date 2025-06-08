#include "../includes/Webserv.hpp"

Webserv::Webserv(std::vector<Server*> list_servers) : _list_servers(list_servers) {
	std::cout << "\033[32m[INFO] Server is ready to run\033[0m" << std::endl;
}

Webserv::~Webserv() {
	std::cout << "\033[32m[INFO] Server is killed\033[0m" << std::endl;
}

// Cette fonction sert à créer et set-up les fds correspondant à chaque virtual host
void Webserv::createEndpoints()
{
	for (size_t i = 0; i < _list_servers.size(); ++i)
	{
		t_endpoint ep;

		// Création du socket
		ep.socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (ep.socketFd < 0)
			throw std::runtime_error("Erreur lors de la création du socket");

		// setsockopt pour réutiliser l'adresse
		int opt = 1;
		if (setsockopt(ep.socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Erreur setsockopt()");

		// Initialisation de sockaddr_in pour Bind
		std::memset(&ep.addr, 0, sizeof(ep.addr));
		ep.addr.sin_family = AF_INET;
		ep.addr.sin_addr.s_addr = htonl(INADDR_ANY); // à vérifier
		ep.addr.sin_port = htons(_list_servers[i]->getPort());

		// Bind
		if (bind(ep.socketFd, reinterpret_cast<sockaddr*>(&ep.addr), sizeof(ep.addr)) < 0)
			throw std::runtime_error("Erreur lors du bind");

		// Listen
		if (listen(ep.socketFd, SOMAXCONN) < 0)
			throw std::runtime_error("Erreur listen");

		_endpoints.push_back(ep);
	}
}


void Webserv::run() {
	std::cout << "\033[32m[INFO] Server is running\033[0m" <<std::endl;

	createEndpoints();

}
