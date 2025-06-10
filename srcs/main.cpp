#include "../includes/main.hpp"

//debug function called in main for see parsing (parsing actually hardcoded)
void printServerVector(std::vector<Server*> servers) {
	std::cout << "\n===== SERVER CONFIGURATION =====" << std::endl;
	std::cout << "Number of servers: " << servers.size() << std::endl;

	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << "\n--- Server " << i + 1 << " ---" << std::endl;
			std::cout << "Server Name: " << servers[i]->getServerName() << std::endl;
		std::cout << "Host: " << servers[i]->getHost() << std::endl;
		std::cout << "Port: " << servers[i]->getPort() << std::endl;
		std::cout << "Root: " << servers[i]->getRoot() << std::endl;
		std::cout << "Index: " << servers[i]->getIndex() << std::endl;

		std::cout << "Allowed Methods: ";
		const std::vector<std::string>& methods = servers[i]->getAllowedMethods();
		if (methods.empty()) {
			std::cout << "None";
		} else {
			for (size_t j = 0; j < methods.size(); ++j) {
				std::cout << methods[j];
				if (j < methods.size() - 1) {
					std::cout << ", ";
				}
			}
		}
		std::cout << std::endl;

		const std::map<std::string, Location>& locations = servers[i]->getLocations();
		std::cout << "Locations: " << locations.size() << std::endl;

		if (!locations.empty()) {
			for (std::map<std::string, Location>::const_iterator it = locations.begin();
				it != locations.end(); ++it) {
				std::cout << "  Location Path: " << it->first << std::endl;
				std::cout << "    Root: " << it->second.root << std::endl;
				std::cout << "    Index: " << it->second.index << std::endl;
				std::cout << "    Alias: " << (it->second.alias ? "Yes" : "No") << std::endl;
				std::cout << "    Client Body Buffer Size: " << it->second.client_body_buffer_size << std::endl;
				std::cout << "    CGI Pass: " << it->second.cgi_pass << std::endl;

				std::cout << "    Allowed Methods: ";
				if (it->second.allowed_methods.empty()) {
					std::cout << "None";
				} else {
					for (size_t j = 0; j < it->second.allowed_methods.size(); ++j) {
						std::cout << it->second.allowed_methods[j];
						if (j < it->second.allowed_methods.size() - 1) {
							std::cout << ", ";
						}
					}
				}
				std::cout << std::endl;

				if (!it->second.nested_locations.empty()) {
					std::cout << "    Nested Locations: " << it->second.nested_locations.size() << std::endl;
				}
			}
		}
	}
	std::cout << "\n===============================" << std::endl;
}

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
			std::cerr << e.what() << '\n';
		}
	} else {
		std::cout << "\033[1;34mUsage: \033[0m" << av[0] << " \033[1;32m<configuration_file>\033[0m" << std::endl;
		std::cout << "\033[1;33mExample: \033[0m" << av[0] << " config/default.conf" << std::endl;
	}
	return 0;
}
