/**
 * @file main.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief The entrypoint to our program
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include "network.hpp"
#include "common.hpp"

static int checkErr(std::string funcName, int retValue)
{
    if (retValue == -1)
    {
        std::cout << funcName << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return (retValue);
}

/**
 * @brief Entrypoint to our program
 *
 * @return int Exit code
 */
int main(void)
{
	sockaddr_storage their_addr;
    socklen_t addr_size;
    addrinfo *servInfo;
	addrinfo hints;
    addrinfo *p;
	int status;
    int sock_fd;
    int new_fd;
    int bytes_sent;
    int yes = 1;
    
	// std::fill(&hints, &hints + sizeof(hints), 0); // cpp alternative for memset, yet to test
	memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me

	if ((status = getaddrinfo(NULL, PORT, &hints, &servInfo)) != 0) // first param is host IP/name and its null because we set ai_flags as AI_PASSIVE
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl; // make this an exception or make a common error class later
		exit(EXIT_FAILURE);
    }
	std::cout << "getaddrinfo successful!" << std::endl;
	
    // go through servInfo linkedlist and bind to the first one that works
    for (p = servInfo; p != NULL; p = p->ai_next)
    {
        if ((sock_fd = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol)) == -1)
            std::cout << "socket: " << strerror(errno) << std::endl;
        else
        {
            checkErr("setsockopt", setsockopt(sock_fd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)));
            // binding the socket to a port means - the port number will be used by the kernel to match an incoming packet to webserv's process socket descriptor. 
            if (bind(sock_fd, servInfo->ai_addr, servInfo->ai_addrlen) != -1)
                break;
            std::cout << "bind: " << strerror(errno) << std::endl;
            close(sock_fd);
        }
    }
    if (!p)
    {
        std::cout << "webserv: failed to bind" << std::endl;
        exit(EXIT_FAILURE);
    }

    // sock_fd = checkErr("socket", socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol));
    // checkErr("setsockopt", setsockopt(sock_fd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)));
    // checkErr("bind", bind(sock_fd, servInfo->ai_addr, servInfo->ai_addrlen)); // rets -1 on error and sets errno

    freeaddrinfo(servInfo);
    
    checkErr("listen", listen(sock_fd, QUEUE_LIMIT));
    addr_size = sizeof(their_addr);
    std::cout << "Listening on port " << PORT << std::endl;
    new_fd = checkErr("accept", accept(sock_fd, (sockaddr *)&their_addr, &addr_size));

    // we can fork here to handle the connection
    std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 100\r\n\r\n<!DOCTYPE html><html><head><title>Hello World</title></head><body><h1>Hello World</h1></body></html>";
    std::string msg2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean venenatis dolor neque. Maecenas vel magna id nibh tristique aliquam. Etiam venenatis elit ultrices, iaculis nulla ut, gravida ligula. Sed at rhoncus turpis. Curabitur suscipit augue quis nisi vulputate, ac porttitor arcu ornare. Nullam suscipit maximus felis sit amet bibendum. Etiam nec tincidunt neque. Nunc purus urna, mollis sit amet accumsan et, finibus ut velit. Mauris suscipit ac augue a volutpat. Nulla a nulla non enim consequat pharetra. Nunc ut pharetra leo. Sed eu varius ante, sed aliquam massa. Cras placerat lorem sit amet leo euismod rhoncus. Donec fermentum est dui, vel imperdiet leo dictum sed. Nam tincidunt purus at porttitor viverra. Integer porta lacinia gravida. Curabitur maximus nibh sem, eu ullamcorper lacus hendrerit non. Ut lorem libero, posuere et massa id, ornare pulvinar dui. Aliquam sodales nisl neque. Donec convallis mi nec elit suscipit rhoncus. Maecenas scelerisque, ipsum non vestibulum placerat, orci enim porta nibh, sed lacinia odio orci ut urna. In eu nulla euismod, fringilla nulla non, scelerisque massa. Curabitur leo eros, iaculis quis pulvinar sed, dictum id diam. Aenean ultrices tellus a enim aliquam varius. Nam semper urna massa, eu dictum lacus ornare id. Nam in nunc sem. Praesent molestie eget massa eget suscipit. Curabitur non lorem a turpis feugiat sagittis. In nulla ante, porta sed tincidunt eu, vestibulum sit amet mi. Sed et neque mi. Sed at urna lacinia, suscipit enim vel, posuere tellus. Nunc in imperdiet dolor, in fermentum eros. Sed blandit nisi a ultrices venenatis. Aenean ut purus nec enim volutpat tristique sit amet eu orci. Morbi consectetur velit vel massa rhoncus sollicitudin. In nisi tellus, pretium sed libero vel, consequat condimentum quam. Proin interdum tellus sed massa pharetra lobortis. Sed vel elementum lorem, vitae tincidunt leo. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Duis non ipsum ornare accumsan.";

    checkErr("send", bytes_sent = send(new_fd, msg.c_str(), msg.length(), 0));
    std::cout << "bytes sent = " << bytes_sent << ", msg len = " << msg.length() << std::endl;
    
    char *buf = new char[2000];
    checkErr("recv", bytes_sent = recv(new_fd, buf, 2000, 0));
    std::cout << "bytes rec = " << bytes_sent << ", msg = " << std::endl << buf << std::endl;
    delete[] buf;

    close(new_fd);
    close(sock_fd);
	return 0;
}
