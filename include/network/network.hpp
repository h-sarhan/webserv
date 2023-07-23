/**
 * @file network.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief This file contains all the necessary headers for networking
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define PORT         "1234"
#define HW_HTML                                                                                    \
    "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: "                \
    "100\r\n\r\n<!DOCTYPE html><html><head><title>Hello World</title></head><body><h1>Hello "      \
    "World</h1></body></html>"

#endif
