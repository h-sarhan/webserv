/**
 * @file Server.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of Server.hpp class
 * @date 2023-07-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Server.hpp"

Server::Server() : name("webserv.com"), port("1234"), sockFd(-1)
{
}

Server::Server(std::string name, std::string port) : name(name), port(port)
{
    addrinfo hints;
    int status;

    // std::fill(&hints, &hints + sizeof(hints), 0); // cpp alternative for
    // memset, yet to test
    memset(&hints, 0, sizeof(hints));   // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // fill in my IP for me
    if ((status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo)) !=
        0)   // first param is host IP/name and its null because we set ai_flags
             // as AI_PASSIVE
        throw SystemCallException("getaddrinfo", gai_strerror(status));
}

static int checkErr(std::string funcName, int retValue)
{
    if (retValue == -1)
        throw SystemCallException(funcName, strerror(errno));
    return (retValue);
}

void Server::bindSocket()
{
    addrinfo *p;
    int reusePort = 1;

    for (p = servInfo; p != NULL; p = p->ai_next)
    {
        if ((this->sockFd = socket(servInfo->ai_family, servInfo->ai_socktype,
                                   servInfo->ai_protocol)) == -1)
            std::cout << "socket: " << strerror(errno) << std::endl;
        else
        {
            checkErr("setsockopt",
                     setsockopt(this->sockFd, SOL_SOCKET, SO_REUSEADDR,
                                &reusePort, sizeof(reusePort)));
            if (bind(this->sockFd, servInfo->ai_addr, servInfo->ai_addrlen) !=
                -1)   // binding the socket to a port means - the port number
                      // will be used by the kernel to match an incoming packet
                      // to webserv's process socket descriptor.
                break;
            std::cout << "bind: " << strerror(errno) << std::endl;
            close(this->sockFd);
        }
    }
    if (!p)
        throw SystemCallException("failed to bind");
}

// just a sample connection handler for now
static void handleConnection(int newFd)
{
    int bytes_sent;

    std::string msg =
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; "
        "charset=UTF-8\r\nContent-Length: 100\r\n\r\n<!DOCTYPE "
        "html><html><head><title>Hello World</title></head><body><h1>Hello "
        "World</h1></body></html>";
    std::string msg2 =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean "
        "venenatis dolor neque. Maecenas vel magna id nibh tristique aliquam. "
        "Etiam venenatis elit ultrices, iaculis nulla ut, gravida ligula. Sed "
        "at rhoncus turpis. Curabitur suscipit augue quis nisi vulputate, ac "
        "porttitor arcu ornare. Nullam suscipit maximus felis sit amet "
        "bibendum. Etiam nec tincidunt neque. Nunc purus urna, mollis sit amet "
        "accumsan et, finibus ut velit. Mauris suscipit ac augue a volutpat. "
        "Nulla a nulla non enim consequat pharetra. Nunc ut pharetra leo. Sed "
        "eu varius ante, sed aliquam massa. Cras placerat lorem sit amet leo "
        "euismod rhoncus. Donec fermentum est dui, vel imperdiet leo dictum "
        "sed. Nam tincidunt purus at porttitor viverra. Integer porta lacinia "
        "gravida. Curabitur maximus nibh sem, eu ullamcorper lacus hendrerit "
        "non. Ut lorem libero, posuere et massa id, ornare pulvinar dui. "
        "Aliquam sodales nisl neque. Donec convallis mi nec elit suscipit "
        "rhoncus. Maecenas scelerisque, ipsum non vestibulum placerat, orci "
        "enim porta nibh, sed lacinia odio orci ut urna. In eu nulla euismod, "
        "fringilla nulla non, scelerisque massa. Curabitur leo eros, iaculis "
        "quis pulvinar sed, dictum id diam. Aenean ultrices tellus a enim "
        "aliquam varius. Nam semper urna massa, eu dictum lacus ornare id. Nam "
        "in nunc sem. Praesent molestie eget massa eget suscipit. Curabitur "
        "non lorem a turpis feugiat sagittis. In nulla ante, porta sed "
        "tincidunt eu, vestibulum sit amet mi. Sed et neque mi. Sed at urna "
        "lacinia, suscipit enim vel, posuere tellus. Nunc in imperdiet dolor, "
        "in fermentum eros. Sed blandit nisi a ultrices venenatis. Aenean ut "
        "purus nec enim volutpat tristique sit amet eu orci. Morbi consectetur "
        "velit vel massa rhoncus sollicitudin. In nisi tellus, pretium sed "
        "libero vel, consequat condimentum quam. Proin interdum tellus sed "
        "massa pharetra lobortis. Sed vel elementum lorem, vitae tincidunt "
        "leo. Orci varius natoque penatibus et magnis dis parturient montes, "
        "nascetur ridiculus mus. Duis non ipsum ornare accumsan.";

    checkErr("send", bytes_sent = send(newFd, msg.c_str(), msg.length(), 0));
    std::cout << "bytes sent = " << bytes_sent << ", msg len = " << msg.length()
              << std::endl;

    char *buf = new char[2000];
	bzero(buf, 2000);
    checkErr("recv", bytes_sent = recv(newFd, buf, 2000, 0));
    std::cout << "bytes rec = " << bytes_sent << ", msg = " << std::endl
              << buf << std::endl;
    delete[] buf;
    close(newFd);
}

void Server::startListening()
{
    int newFd;
    sockaddr_storage their_addr;
    socklen_t addr_size;

    checkErr("listen", listen(this->sockFd, QUEUE_LIMIT));
    addr_size = sizeof(their_addr);
    std::cout << "Listening on port " << this->port << std::endl;
    newFd = checkErr(
        "accept", accept(this->sockFd, (sockaddr *) &their_addr, &addr_size));
    handleConnection(newFd);
}

Server::~Server()
{
    freeaddrinfo(servInfo);
    if (sockFd != -1)
        close(sockFd);
}
