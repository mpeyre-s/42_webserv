#include "../includes/main.hpp"

<<<<<<< HEAD
=======
void printLocation(const Location *location, int indentLevel = 1) {
	std::string indent(indentLevel * 8, ' ');
	std::string subIndent((indentLevel + 1) * 4, ' ');

	std::cout << indent << "Location Path: " << location->path << std::endl;

	std::cout << indent << "Allowed Methods: ";
	for (size_t i = 0; i < location->allowed_methods.size(); i++) {
		std::cout << location->allowed_methods[i];
		if (i < location->allowed_methods.size() - 1) std::cout << ", ";
	}
	std::cout << std::endl;

	std::cout << indent << "Root: " << location->root << std::endl;
	std::cout << indent << "Index: " << location->index << std::endl;
	std::cout << indent << "Upload Directory: " << location->upload_dir << std::endl;
	std::cout << indent << "Auto Index: " << (location->auto_index ? "on" : "off") << std::endl;
	std::cout << indent << "Client Max Body Size: " << location->client_max_body_size << std::endl;

	std::cout << indent << "CGI Extensions: ";
	for (size_t i = 0; i < location->cgi_extensions.size(); i++) {
		std::cout << location->cgi_extensions[i];
		if (i < location->cgi_extensions.size() - 1) std::cout << ", ";
	}
	std::cout << std::endl;

	std::cout << indent << "CGI Path: " << location->cgi_path << std::endl;
	std::cout << indent << "Redirect Code: " << location->redirect_code << std::endl;
	std::cout << indent << "Redirect URL: " << location->redirect_url << std::endl;

	if (!location->error_pages.empty()) {
		std::cout << indent << "Error Pages:" << std::endl;
		for (std::map<int, std::string>::const_iterator error = location->error_pages.begin(); error != location->error_pages.end(); ++error) {
			std::cout << subIndent << error->first << " -> " << error->second << std::endl;
		}
	}
}

void printServerVector(const std::vector<Server*>& servers) {
	std::cout << "\n===== SERVER CONFIGURATION =====" << std::endl;
	std::cout << "Number of servers: " << servers.size() << std::endl;

	for (size_t i = 0; i < servers.size(); i++) {
		Server* server = servers[i];
		std::cout << "\n----- Server " << i + 1 << (server->getDefaultStatus() ? " (Default)" : "") << " -----" << std::endl;
		std::cout << "Server Name: " << server->getServerName() << std::endl;
		std::cout << "Host: " << server->getHost() << std::endl;
		std::cout << "Port: " << server->getPort() << std::endl;
		std::cout << "Root: " << server->getRoot() << std::endl;
		std::cout << "Index: " << server->getIndex() << std::endl;
		std::cout << "Auto Index: " << server->getAutoIndex() << std::endl;
		std::cout << "Max Body Size: " << server->getMaxBodySize() << std::endl;

		std::cout << "Allowed Methods: ";
		const std::vector<std::string>& methods = server->getAllowedMethods();
		for (size_t j = 0; j < methods.size(); j++) {
			std::cout << methods[j];
			if (j < methods.size() - 1) std::cout << ", ";
		}
		std::cout << std::endl;

		std::cout << "Error Pages: ";
		const std::map<int, std::string>& error_pages = server->getErrorPages();
		for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
			std::cout << "[" << it->first << "] " << it->second << std::endl;
		}
		std::cout << std::endl;

		const std::map<std::string, Location*>& locations = server->getLocations();
		if (!locations.empty()) {
			std::cout << "Locations:" << std::endl;
			for (std::map<std::string, Location*>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
				std::cout << it->first << std::endl;
				printLocation(it->second);
			}
		}
	}

	std::cout << "\n===============================" << std::endl;
}

>>>>>>> good-parsing
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
<<<<<<< HEAD
			std::cerr << e.what() << std::endl;
=======
			std::cerr << "\033[1;31m[ERROR]\033[1m " << e.what() << "\033[0m" << '\n';
>>>>>>> good-parsing
		}
	} else {
		std::cout << "\033[1;34mUsage: \033[0m" << av[0] << " \033[1;32m<configuration_file>\033[0m" << std::endl;
		std::cout << "\033[1;33mExample: \033[0m" << av[0] << " config/default.conf" << std::endl;
	}
	return 0;
}
