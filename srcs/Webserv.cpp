#include "../includes/Webserv.hpp"

Webserv::Webserv(std::vector<Server*> list_servers) : _list_servers(list_servers) {}

Webserv::~Webserv() {}

void Webserv::run() {
	std::cout << "Server '" << _list_servers[0]->getServerName() << "' started succesfully!" <<std::endl;
}
