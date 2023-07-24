/**
 * @file ServerInfo.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of ServerInfo class
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "network/ServerInfo.hpp"

/**
 * @brief Constructs a new ServerInfo object for TCP sockets and localhost
 *
 */
ServerInfo::ServerInfo() : hints()
{
    info = NULL;
    // hints may have to be bzeroed here if the above initialization doesnt really make it 0
    hints.ai_family = AF_UNSPEC;       // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;   // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;       // fill in my IP for me
}

/**
 * @brief Fills serverInfo object with address info of the port
 * 		  Throws a SystemCallException on failure

 * @param port port we want to listen on
 */
void ServerInfo::getServerInfo(int port)
{
    int status;
    std::stringstream ss;
    std::string portStr;

    ss << port;
    ss >> portStr;
    status = getaddrinfo(NULL, portStr.c_str(), &hints, &info);
    if (status != 0)
        throw SystemCallException("getaddrinfo", gai_strerror(status));
}

/**
 * @brief Binds a new socket with the port set in ServerInfo::info
 * 		  The port number will be used by the kernel to match incoming packets to this fd
 * @return socket file descriptor that refers to the listener on that port
 */
int ServerInfo::bindSocketToPort()
{
    addrinfo *p;
    int reusePort = 1;
    int listenerFd;

    for (p = info; p != NULL; p = p->ai_next)
    {
        listenerFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenerFd == -1)
            Log(ERR) << "socket: " << strerror(errno) << std::endl;
        else
        {
            // Setting the socket to be non-blocking
            SystemCallException::checkErr("fcntl", fcntl(listenerFd, F_SETFL, O_NONBLOCK));
            SystemCallException::checkErr(
                "setsockopt",
                setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &reusePort, sizeof(reusePort)));
            if (bind(listenerFd, p->ai_addr, p->ai_addrlen) != -1)
                break;
            Log(WARN) << "bind: " << strerror(errno) << std::endl;
            close(listenerFd);
        }
    }
    if (!p)
        throw SystemCallException("failed to bind");
    SystemCallException::checkErr("listen", listen(listenerFd, BACKLOG));
    return (listenerFd);
}

/**
 * @brief Destroy the ServerInfo object - frees addrinfo struct info
 *
 */
ServerInfo::~ServerInfo()
{
    if (info)
        freeaddrinfo(info);
}
