#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define MAX_BODY_SIZE 10 // Mo

std::vector<std::string> split(const std::string &str, std::string delimiter);
bool validIpFormat(std::string &str);
std::string intToStdString(int nb);
bool checkFileExtension(const std::string &path, const std::string &extension);
