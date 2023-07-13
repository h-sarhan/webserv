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

bool quit = false;

// Server::Server() : port("1234"), listener(-1)
// {
//     addrinfo hints = {};
//     int status;

//     hints.ai_family = AF_UNSPEC;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_flags = AI_PASSIVE;
//     status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo);
//     if (status != 0)
//         throw SystemCallException("getaddrinfo", gai_strerror(status));
    
// }

Server::Server(std::string port, serverList virtualServers) : port(port), listener(-1), virtualServers(virtualServers)
{
    addrinfo hints = {};
    int status;

    hints.ai_family = AF_UNSPEC;       // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;   // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;       // fill in my IP for me
    status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo);
    if (status != 0)
        throw SystemCallException("getaddrinfo", gai_strerror(status));
    std::cout << "Virtual servers - " << std::endl;
    std::cout << virtualServers << std::endl;
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
    (void)virtualServers;
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
    sockets.push_back(createPollFd(this->listener, POLLIN));
    checkErr("listen", listen(this->listener, QUEUE_LIMIT));
}

// change this to use fd directly and close stuff in caller func
void Server::readRequest(size_t clientNo)
{
    size_t contentLen = 200000; // temporary contentLen until the one from the request is parsed
    char *buf = new char[contentLen];
    std::string &req = cons.at(sockets[clientNo].fd).request;
    size_t &totalRec = cons.at(sockets[clientNo].fd).totalBytesRec;
    int bytesRec;

    // receiving request
    std::cout << "Received a request: " << std::endl;
    bytesRec = recv(sockets[clientNo].fd, buf, contentLen, 0);
    if (bytesRec < 0)
        std::cout << "Failed to receive request: " << strerror(errno) << std::endl;
    else if (bytesRec == 0)
        std::cout << "Connection closed by client" << std::endl;
    if (bytesRec <= 0)
    {
        close(sockets[clientNo].fd);
        cons.erase(sockets[clientNo].fd);
        sockets.erase(sockets.begin() + clientNo);
        delete[] buf;
        return ;
    }
    buf[bytesRec] = 0;
    totalRec += bytesRec;
    req += buf;
    // if this is a POST req and totalRec != contentLength
        // append buf to request;
        // delete[] buf
        // return here so that we dont set the events to start responding
    std::cout << "Request size = " << totalRec << std::endl;
    totalRec = 0; // getting ready for next request
    sockets[clientNo].events = POLLIN | POLLOUT;
    std::cout << "bytes = " << bytesRec << ", msg: " << std::endl;
    std::cout << "---------------------------------------------------------\n";
    std::cout << buf;
    std::cout << "---------------------------------------------------------\n";
    delete[] buf;
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

void Server::sendResponse(size_t clientNo)
{
    ssize_t bytesSent;
    std::string &req = cons.at(sockets[clientNo].fd).request;
    std::string &res = cons.at(sockets[clientNo].fd).response;
    size_t &totalSent = cons.at(sockets[clientNo].fd).totalBytesSent;

    if (req.length() > 0)
    {
        res = createResponse("assets/artgallery.html", HTTP_HEADERS);
        totalSent = 0;
        req.clear();
    }
    if (res.length() == 0)
    {
        std::cout << "Nothing to send >:(" << std::endl;
        return ;
    }
    std::cout << "Sending a response... " << std::endl;
    bytesSent = send(sockets[clientNo].fd, res.c_str() + totalSent, res.length() - totalSent, 0);
    if (bytesSent < 0)
        std::cout << "Sending response failed: " << strerror(errno) << std::endl;
    else
    {
        totalSent += bytesSent;
        if (totalSent < res.length()) // partial send 
        {
            std::cout << "Response only sent partially: " << bytesSent << ". Total: " << totalSent << std::endl;
            return ;
        }
        else // everything got sent
        {
            std::cout << "Response sent successfully to fd " << clientNo << ", " << sockets[clientNo].fd << ", total bytes sent = " << totalSent << std::endl;
            // if keep-alive was requested 
                // clear response string, set totalBytesSent to 0 and return here!!
        }
    }
    close(sockets[clientNo].fd);
    cons.erase(sockets[clientNo].fd);
    sockets.erase(sockets.begin() + clientNo);
    std::cout << "---------------------------------------------------------\n";
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
    fcntl(newFd, F_SETFL, O_NONBLOCK); // enable this when doing partial recv
    std::cout << "New connection! fd = " << newFd << std::endl;
    if (sockets.size() < MAX_CLIENTS)
    {
        cons.insert(std::make_pair(newFd, Connection()));
        sockets.push_back(createPollFd(newFd, POLLIN));
    }
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
    std::cout << "Listening on port " << this->port << std::endl;
    while (true)
    {
        checkErr("poll", poll(&sockets[0], sockets.size(), -1));
        for (size_t i = 0; i < sockets.size(); i++)
        {
            // server listener got something new to read
            if (sockets[i].revents & POLLIN)
            {
                if (sockets[i].fd == listener)
                    acceptNewConnection();
                else // its one of the clients
                    readRequest(i);
            }
            else if (sockets[i].revents & POLLOUT)
                sendResponse(i);
        }
        if (quit)
            break;
    }
}

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
    freeaddrinfo(servInfo);
    for (size_t i = 0; i < sockets.size(); i++)
        close(sockets[i].fd);
}
