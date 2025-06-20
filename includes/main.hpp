#pragma once

#include <iostream>
#include <vector>
#include <csignal>

#include "Server.hpp"
#include "ConfigParsing.hpp"
#include "Webserv.hpp"

extern volatile sig_atomic_t g_stop;

void handleSigint(int signum);
