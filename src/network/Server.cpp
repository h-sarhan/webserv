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
int g_sockFd = -1;

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
            // fcntl(this->sockFd, F_SETFL, O_NONBLOCK); // Setting the socket to be non-blocking
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
    g_sockFd = this->sockFd;
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
    std::string msg2 = LONG_MSG;
    char *buf = new char[2000];

    // receiving request
    std::cout << "Received a request: " << std::endl;
	bzero(buf, 2000);
    checkErr("recv", bytes_sent = recv(newFd, buf, 2000, 0));
    std::cout << "bytes = " << bytes_sent << ", msg: " << std::endl
              << buf << std::endl;
    delete[] buf;
    
    // sending a response
    std::cout << "Sending a response: " << std::endl;
    checkErr("send", bytes_sent = send(newFd, msg.c_str(), msg.length(), 0));
    std::cout << "bytes = " << bytes_sent << ", msg len = " << msg.length()
              << std::endl << std::endl;
    close(newFd);
}

static void sig_int_handler(int sigNo)
{
    (void)sigNo;
    close(g_sockFd);
}

void Server::startListening()
{
    int newFd;
    sockaddr_storage their_addr;
    socklen_t addr_size;
    
    signal(SIGINT, sig_int_handler);
    checkErr("listen", listen(this->sockFd, QUEUE_LIMIT));
    addr_size = sizeof(their_addr);
    std::cout << "Listening on port " << this->port << std::endl;
    while (true)
    {
        std::cout << "Waiting for a request" << std::endl;
        newFd = checkErr(
            "accept", accept(this->sockFd, (sockaddr *) &their_addr, &addr_size));
        handleConnection(newFd);
    }
}

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
    freeaddrinfo(servInfo);
    if (sockFd != -1)
        close(sockFd);
}
