#include "../includes/Webserv.hpp"

Webserv::Webserv(std::vector<Server*> list_servers) : _list_servers(list_servers) {
	std::cout << "\033[32m[INFO] Server is ready to run\033[0m" << std::endl;
}

Webserv::~Webserv() {
	std::cout << "\033[32m[INFO] Server is killed\033[0m" << std::endl;
}

void Webserv::run() {
	std::cout << "\033[32m[INFO] Server is running\033[0m" <<std::endl;
}
