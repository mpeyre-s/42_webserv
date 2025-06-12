#include "../includes/main.hpp"

int main(int ac, char **av) {
	if (ac == 2) {
		try {
			std::string configFile = av[1];
			ConfigParsing configParsing(configFile);

			std::vector<Server*> list_servers = configParsing.createServerList();

			Webserv webserv(list_servers);
			webserv.run();
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	} else {
		std::cout << "\033[1;34mUsage: \033[0m" << av[0] << " \033[1;32m<configuration_file>\033[0m" << std::endl;
		std::cout << "\033[1;33mExample: \033[0m" << av[0] << " config/default.conf" << std::endl;
	}
	return 0;
}
