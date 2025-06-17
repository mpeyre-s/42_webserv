#include "../includes/utils.hpp"

std::vector<std::string> split(const std::string &str, std::string delimiter) {
	std::vector<std::string> result;
	size_t start_pos = 0;
	size_t end_pos = str.find(delimiter);
	while (end_pos != std::string::npos) {
		result.push_back(str.substr(start_pos, end_pos - start_pos));
		start_pos = end_pos + 1;
		end_pos = str.find(delimiter, start_pos);
	}
	if (start_pos < str.length()) {
		result.push_back(str.substr(start_pos));
	}
	return result;
}

bool validIpFormat(std::string &str) {
	size_t colon = str.find(':');
	if (colon == std::string::npos)
		return false;

	std::string ip = str.substr(0, colon);
	std::string port_str = str.substr(colon + 1);

	std::vector<std::string> parts = split(ip, ".");
	if (parts.size() != 4)
		return false;
	for (size_t i = 0; i < 4; ++i) {
		if (parts[i].empty() || parts[i].length() > 3)
			return false;
		for (size_t j = 0; j < parts[i].length(); ++j) {
			if (!isdigit(parts[i][j]))
				return false;
		}
		int num = std::atoi(parts[i].c_str());
		if (num < 0 || num > 255)
			return false;
	}

	if (port_str.empty())
		return false;
	for (size_t i = 0; i < port_str.length(); ++i) {
		if (!isdigit(port_str[i]))
			return false;
	}
	int port = std::atoi(port_str.c_str());
	if (port < 1 || port > 65535)
		return false;

	return true;
}

std::string intToStdString(int nb) {
	std::ostringstream oss117;
	oss117 << nb;
	return oss117.str();
}

bool checkFileExtension(const std::string &path, const std::string &extension) {
	if (extension.length() > path.length())
		return false;
	return path.compare(path.length() - extension.length(), extension.length(), extension) == 0;
}
