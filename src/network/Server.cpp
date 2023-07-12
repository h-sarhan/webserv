/**
 * @file Server.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of Server.hpp class
 * @date 2023-07-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "network/Server.hpp"
#include "network/network.hpp"
#include <cstddef>

bool quit = false;
Server::Server() : name("webserv.com"), port("1234"), listener(-1)
{
    addrinfo hints = {};
    int status;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo);
    if (status != 0)
        throw SystemCallException("getaddrinfo", gai_strerror(status));
    // virtualServers = getConfig();
}

Server::Server(std::string name, std::string port) : name(name), port(port), listener(-1)
{
    addrinfo hints = {};
    int status;

    hints.ai_family = AF_UNSPEC;       // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;   // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;       // fill in my IP for me
    status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo);
    if (status != 0)
        throw SystemCallException("getaddrinfo", gai_strerror(status));
    // virtualServers = getConfig();
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
        this->listener = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
        if (this->listener == -1)
            std::cout << "socket: " << strerror(errno) << std::endl;
        else
        {
            // Setting the socket to be non-blocking
            fcntl(this->listener, F_SETFL, O_NONBLOCK);
            checkErr("setsockopt", setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR, &reusePort,
                                              sizeof(reusePort)));
            // binding the socket to a port means - the port number
            // will be used by the kernel to match an incoming packet
            // to webserv's process socket descriptor.
            if (bind(this->listener, servInfo->ai_addr, servInfo->ai_addrlen) != -1)
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
std::string Server::readRequest(size_t clientNo)
{
    int bytes_rec;
    char *buf = new char[2000000];
    std::string request;

    // receiving request
    std::cout << "Received a request: " << std::endl;
    bytes_rec = recv(clients[clientNo].fd, buf, 2000000, 0);
    if (bytes_rec < 0)
        std::cout << "Failed to receive request" << std::endl;
    else if (bytes_rec == 0)
        std::cout << "Connection closed by client" << std::endl;
    if (bytes_rec <= 0)
    {
        close(clients[clientNo].fd);
        clients.erase(clients.begin() + clientNo);
        delete[] buf;
        return std::string();
    }
    buf[bytes_rec] = 0;
    std::cout << "bytes = " << bytes_rec << ", msg: " << std::endl;
    std::cout << "---------------------------------------------------------\n";
    std::cout << buf;
    std::cout << "---------------------------------------------------------\n";

    // example of how to save an image from a post req or any file

    // std::ofstream file("img.png", std::ios::binary);
    // std::string req(buf);
    // int index = req.find("\r\n\r\n");
    // file.write(buf + index + 4, bytes_rec - index - 4);
    // file.close();
    request = buf;
    delete[] buf;
    return (request);
}

std::string static createResponse(std::string filename, std::string headers)
{
    std::ifstream file;
    std::stringstream responseBuffer;
    std::stringstream fileBuffer;
    std::string fileContents;

    file.open(filename.c_str());
    // if (!file.good())
        // return 404 page as response
    fileBuffer << file.rdbuf();
    file.close();
    fileContents = fileBuffer.str();
    responseBuffer << headers;
    responseBuffer << fileContents.length() << "\r\n\r\n";
    responseBuffer << fileContents;
    return responseBuffer.str();
}

static int sendAll(int clientFd, std::string& msg, size_t msgLen)
{
    ssize_t bytesLeft = msgLen;
    ssize_t bytesSent = 0;

    while (bytesLeft > 0)
    {
        bytesSent = send(clientFd, msg.c_str(), bytesLeft, 0);
        if (bytesSent == -1)
            return (-1);
        bytesLeft -= bytesSent;
    }
    return (bytesSent);
}

void Server::sendResponse(size_t clientNo, std::string request)
{
    std::string msg;
    int bytesSent;

    if (request.length() == 0)
        return;
    if (request.find("/artgallerycontent/2020_3.jpg") != std::string::npos)
        msg = createResponse("artgallerycontent/2020_3.jpg", IMG_HEADERS);
    else
        msg = createResponse("artgallery.html", HTTP_HEADERS);
    std::cout << "Sending a response... " << std::endl;
    bytesSent = sendAll(clients[clientNo].fd, msg, msg.length());
    if (bytesSent < 0)
        std::cout << "Sending response failed" << std::endl;
    else
        std::cout << "Response sent to fd " << clientNo << ", " << clients[clientNo].fd
                  << ", bytesSent = " << bytesSent << std::endl;
    std::cout << "---------------------------------------------------------\n";
    close(clients[clientNo].fd);
    clients.erase(clients.begin() + clientNo);
}

static void sigInthandler(int sigNo)
{
    (void) sigNo;
    quit = true;
}

void Server::acceptNewConnection()
{
    int newFd;
    sockaddr_storage their_addr;
    socklen_t addr_size;

    addr_size = sizeof(their_addr);
    newFd = accept(this->listener, (sockaddr *) &their_addr, &addr_size);
    if (newFd == -1)
    {
        std::cout << "Accept failed" << std::endl;
        return;
    }
    std::cout << "New connection! fd = " << newFd << std::endl;
    if (clients.size() < MAX_CLIENTS)
        clients.push_back(createPollFd(newFd, POLLIN | POLLOUT));
    else
    {
        std::cout << "Maximum clients reached, dropping this connection" << std::endl;
        // send 503 error page
        close(newFd);
    }
}

void Server::startListening()
{
    signal(SIGINT, sigInthandler);
    checkErr("listen", listen(this->listener, QUEUE_LIMIT));
    std::cout << "Listening on port " << this->port << std::endl;
    while (true)
    {
        checkErr("poll", poll(&clients[0], clients.size(), -1));
        for (size_t i = 0; i < clients.size(); i++)
        {
            // server listener got something new to read
            if ((clients[i].fd == listener) && (clients[i].revents & POLLIN))
                acceptNewConnection();
            // one of the clients is ready to send and recv
            else if ((clients[i].revents & POLLIN) && (clients[i].revents & POLLOUT))
                sendResponse(i, readRequest(i));
        }
        if (quit)
            break;
    }
}

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
    freeaddrinfo(servInfo);
    for (size_t i = 0; i < clients.size(); i++)
        close(clients[i].fd);
}
