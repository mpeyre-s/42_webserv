#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#define MAX_BODY_SIZE 10 // Mo
class Server;

struct serverGroup {
	std::vector<Server*> list_server;
	size_t nb_server;
	std::string ip;
	int port;
	int fd;
};

std::vector<std::string> split(const std::string &str, std::string delimiter);
bool validIpFormat(std::string &str);
std::string intToStdString(int nb);
bool checkFileExtension(const std::string &path, const std::string &extension);
