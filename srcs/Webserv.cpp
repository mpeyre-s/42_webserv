#include "../includes/Webserv.hpp"

Webserv::Webserv(std::vector<Server*> list_servers) : _list_servers(list_servers) {
	std::cout << "\033[32m[INFO] Server is ready to run\033[0m" << std::endl;

	//std::string buffer_in = "POST /path/resource HTTP/1.1\r\nHost: www.exemple.com\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: fr-FR,fr;q=0.8,en-US;q=0.5,en;q=0.3\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\nparam1=value1&param2=value2";
	std::string buffer_in = "GET /index.html HTTP/1.1\r\nHost: www.exemple.com\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: fr-FR,fr;q=0.8,en-US;q=0.5,en;q=0.3\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
	std::cout << "===== BUFFER_IN =====" << std::endl << buffer_in << std::endl << std::endl;
	Request request(buffer_in);
	Response *response = request.process(_list_servers[0]);
	std::string buffer_out = response->getStringResponse();

	std::cout << "===== BUFFER_OUT =====" << std::endl << buffer_out << std::endl;
}

Webserv::~Webserv() {
	std::cout << "\033[32m[INFO] Server is killed\033[0m" << std::endl;
}

void Webserv::run() {
	std::cout << "\033[32m[INFO] Server is running\033[0m" <<std::endl;
}
