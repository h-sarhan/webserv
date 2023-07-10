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

bool quit = false;
Server::Server() : name("webserv.com"), port("1234"), listener(-1)
{
    addrinfo hints;
    int status;

    // std::fill(&hints, &hints + sizeof(hints), 0); // cpp alternative for
    // memset, yet to test
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo)) != 0)
        throw SystemCallException("getaddrinfo", gai_strerror(status));
}

Server::Server(std::string name, std::string port)
    : name(name), port(port), listener(-1)
{
    addrinfo hints;
    int status;

    // std::fill(&hints, &hints + sizeof(hints), 0); // cpp alternative for
    // memset, yet to test
    memset(&hints, 0, sizeof(hints));   // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // fill in my IP for me
    if ((status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo)) != 0)   // first param is host IP/name and its null because we set ai_flags
        throw SystemCallException("getaddrinfo", gai_strerror(status));
}

static int checkErr(std::string funcName, int retValue)
{
    if (retValue == -1)
        throw SystemCallException(funcName, strerror(errno));
    return (retValue);
}

static pollfd createPollFd(int fd, short events)
{
    pollfd socket;

    socket.fd = fd;
    socket.events = events;
    socket.revents = 0;

    return socket;
}

void Server::bindSocket()
{
    addrinfo *p;
    int reusePort = 1;

    for (p = servInfo; p != NULL; p = p->ai_next)
    {
        if ((this->listener = socket(servInfo->ai_family, servInfo->ai_socktype,
                                     servInfo->ai_protocol)) == -1)
            std::cout << "socket: " << strerror(errno) << std::endl;
        else
        {
            // fcntl(this->listener, F_SETFL, O_NONBLOCK); // Setting the socket
            // to be non-blocking
            checkErr("setsockopt",
                     setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR,
                                &reusePort, sizeof(reusePort)));
            if (bind(this->listener, servInfo->ai_addr, servInfo->ai_addrlen) !=
                -1)   // binding the socket to a port means - the port number
                      // will be used by the kernel to match an incoming packet
                      // to webserv's process socket descriptor.
                break;
            std::cout << "bind: " << strerror(errno) << std::endl;
            close(this->listener);
        }
    }
    if (!p)
        throw SystemCallException("failed to bind");
    clients.push_back(createPollFd(this->listener, POLLIN));
}

// change this to use fd directly and close stuff in caller func
bool Server::readRequest(size_t clientNo)
{
    int bytes_rec;
    char *buf = new char[2000];

    // receiving request
    std::cout << "Received a request: " << std::endl;
    bytes_rec = recv(clients[clientNo].fd, buf, 2000, 0);
    if (bytes_rec < 0)
        std::cout << "Failed to receive request" << std::endl;
    else if (bytes_rec == 0)
        std::cout << "Connection closed by client" << std::endl;
    if (bytes_rec <= 0)
    {
        close(clients[clientNo].fd);
        clients.erase(clients.begin() + clientNo);
        delete[] buf;
        return (false);
    }
    buf[bytes_rec] = 0;
    std::cout << "bytes = " << bytes_rec << ", msg: " << std::endl;
    std::cout << "---------------------------------------------------------\n";
    std::cout << buf << std::endl;
    std::cout << "---------------------------------------------------------\n";
    delete[] buf;
    return (true);
}

void Server::sendResponse(size_t clientNo)
{
    std::ifstream file;
    std::stringstream buffer;
    std::string fileContents;
    std::string msg = HTTP_HEADERS;
    int bytesSent;

    file.open("artgallery.html");
    buffer << file.rdbuf();
    fileContents = buffer.str();
    msg.append(fileContents);
    // std::string msg = HW_HTML;
    std::cout << "Sending a response: " << std::endl;
    if ((bytesSent = send(clients[clientNo].fd, msg.c_str(), msg.length(), 0)) < 0)
        std::cout << "Sending response failed" << std::endl;
    else
        std::cout << "Response sent! bytesSent = " << bytesSent << std::endl;
    std::cout << "been responding to fd = " << clientNo << ", " << clients[clientNo].fd << std::endl;
    close(clients[clientNo].fd);
    std::cout << "clients len b= " << clients.size() << std::endl;
    clients.erase(clients.begin() + clientNo);
    std::cout << "clients len a= " << clients.size() << std::endl;
}

static void sigInthandler(int sigNo)
{
    (void) sigNo;
    quit = true;
}

void Server::startListening()
{
    int newFd;
    int pollCount;
    sockaddr_storage their_addr;
    socklen_t addr_size;

    signal(SIGINT, sigInthandler);
    checkErr("listen", listen(this->listener, QUEUE_LIMIT));
    addr_size = sizeof(their_addr);
    std::cout << "Listening on port " << this->port << std::endl;
    while (true)
    {
        // -1 timeout means wait forever
        std::cout << "Waiting for a request" << std::endl;
        pollCount = checkErr("poll", poll(&clients[0], clients.size(), -1));
		for (size_t i = 0; i < clients.size(); i++)
        {
            if ((clients[i].revents & POLLIN) && (clients[i].fd == listener))   // server listener got something new to read
            {
                std::cout << "New connection!" << std::endl;
                newFd = accept(this->listener, (sockaddr *) &their_addr, &addr_size);
                if (newFd == -1)
                {
                    std::cout << "Accept failed" << std::endl;
                    continue;
                }
                if (clients.size() < MAX_CLIENTS)
                    clients.push_back(createPollFd(newFd, POLLIN | POLLOUT));
                else
                {
                    std::cout << "Maximum clients reached, dropping this connection" << std::endl;
                    close(newFd);
                }
            }
            else if ((clients[i].revents & POLLIN) && (clients[i].revents & POLLOUT))  // one of the clients is ready to be read from
            {
                if (!readRequest(i))
                    continue;
                sendResponse(i);
            }
            else if ((clients[i].revents & POLLIN) && !(clients[i].revents & POLLOUT))
                std::cout << "ready to read, not write, fd = " << i << ", " << clients[i].fd << std::endl;
            else if (!(clients[i].revents & POLLIN) && (clients[i].revents & POLLOUT))
                std::cout << "ready to write, not read, fd = " << i << ", " << clients[i].fd << std::endl;
        }
        if (quit)
            break;
    }
}

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
    freeaddrinfo(servInfo);
    // close all open connection fds
    // delete[] clients;
    if (listener != -1)
        close(listener);
}
