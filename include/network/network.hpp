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

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#define QUEUE_LIMIT 10
#define PORT        "1234"
#define IMG_HEADERS "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Length: "
#define HTTP_HEADERS "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: "
#define HW_HTML "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 100\r\n\r\n<!DOCTYPE html><html><head><title>Hello World</title></head><body><h1>Hello World</h1></body></html>"

// extern bool quit;

#endif
