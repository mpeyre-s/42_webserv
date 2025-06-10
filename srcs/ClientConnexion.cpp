#include "../includes/ClientConnexion.hpp"

ClientConnexion::ClientConnexion(int fd, Server *server) : _fd(fd), _server(server) {
	std::cout << "New Client fd = '"<< fd << "' has been created" << std::endl;
}
