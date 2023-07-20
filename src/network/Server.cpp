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
#include "enums/RequestTypes.hpp"
#include "network/SystemCallException.hpp"
#include "network/network.hpp"
#include "responses/Response.hpp"

bool quit = false;

// Server::Server(serverList virtualServers = std::vector<ServerBlock>(1,
// createDefaultServerBlock()))
Server::Server(serverList virtualServers)
{
    std::cout << "Virtual servers - " << std::endl;
    std::cout << virtualServers << std::endl;
    std::vector<ServerBlock>::iterator it;
    for (it = virtualServers.begin(); it != virtualServers.end(); it++)
    {
        if (portAlreadyInUse(it->port))
            continue;
        std::cout << "Setting up listener on port " << it->port << std::endl;
        initListener(it->port, virtualServers);
    }
}

bool Server::portAlreadyInUse(unsigned int port)
{
    std::map<int, std::vector<ServerBlock *> >::iterator it;
    for (it = configBlocks.begin(); it != configBlocks.end(); it++)
        if (it->second[0]->port == port)
            return true;
    return false;
}

static pollfd createPollFd(int fd, short events)
{
    pollfd socket;

    socket.fd = fd;
    socket.events = events;
    socket.revents = 0;

    return socket;
}

void Server::initListener(unsigned int port, serverList virtualServers)
{
    std::vector<ServerBlock *> config;
    std::vector<ServerBlock>::iterator it;
    ServerInfo servInfo;
    int listenerFd;

    // filtering out the server blocks that use this port
    for (it = virtualServers.begin(); it != virtualServers.end(); it++)
        if (it->port == port)
            config.push_back(it.base());
    servInfo.getServerInfo(port);
    listenerFd = servInfo.bindSocketToPort();
    sockets.push_back(createPollFd(listenerFd, POLLIN));
    configBlocks.insert(std::make_pair(listenerFd, config));
}

void Server::closeConnection(int clientNo)
{
    std::cout << "Closing connection " << sockets[clientNo].fd << std::endl;
    close(sockets[clientNo].fd);
    cons.erase(sockets[clientNo].fd);
    sockets.erase(sockets.begin() + clientNo);
}

void Server::respondToRequest(size_t clientNo)
{
    Connection &c = cons.at(sockets[clientNo].fd);
    int sendStatus;

    c.processRequest(configBlocks.at(c.listener));
    sendStatus = c.response.sendResponse(sockets[clientNo].fd);
    if (sendStatus == IDLE_CONNECTION)
    {
        time_t curTime;
        time(&curTime);
        if (curTime - c.startTime >= c.timeOut)
        {
            std::cout << "Connection timed out! (idle for " << c.timeOut << "s): " << sockets[clientNo].fd << std::endl;
            closeConnection(clientNo);
        }
        return;
    }
    if (sendStatus == SEND_PARTIAL)
        return;
    if (sendStatus == SEND_SUCCESS)
        if (c.keepConnectionAlive())
            return;
    closeConnection(clientNo);
    std::cout << "---------------------------------------------------------\n";
}

static void sigInthandler(int sigNo)
{
    (void) sigNo;
    quit = true;
}

// change this to use fd directly and close stuff in caller func
void Server::readBody(size_t clientNo)
{
    Request &req = cons.at(sockets[clientNo].fd).request;

    if (req.headers().count("content-length"))
    {
        std::istringstream iss(req.headers()["content-length"]);
        size_t contentLen;
        iss >> contentLen;
        // return here so that we dont set the events to start responding
        if (req.requestLength() - req.bodyStart() != contentLen)
        {
            std::cout << "Only " << req.requestLength() << "/" << contentLen << " bytes received" << std::endl;
            return;
        }
    }
    else if (req.headers().count("transfer-encoding"))
    {
        std::string buf = std::string(req.buffer(), req.buffer() + req.requestLength());
        if (buf.rfind("0\r\n\r\n") == std::string::npos)
        {
            std::cout << "Chunked transfer encoding in prog. " << req.requestLength() << " bytes received." << std::endl;
            return;
        }
        // chunked transfer encoding completed
        // handle chunked transfer encoding
        // here req.buffer() may contain a couple of chunks already or at least one
        // my job here would be to extract the chunked data
        // this means to skip over the chunk sizes at the start of a chunk and CRLF at the end

        // if chunk size is not 0, return
        // return here so that we dont set the events to start responding
    }
    std::cout << "Full request size: " << req.requestLength() << std::endl;
    sockets[clientNo].events = POLLIN | POLLOUT;

    std::cout << "---------------------------------------------------------\n";
    std::cout << std::string(req.buffer(), req.buffer() + req.bodyStart() - 1);
    std::cout << "---------------------------------------------------------\n";

    // if this a post req, the only target type we care about is METHOD_NOT_ALLOWED
}

void Server::recvData(size_t clientNo)
{
    Request &req = cons.at(sockets[clientNo].fd).request;
    char *buf;
    ssize_t bytesRec;
    size_t bufSize = 1000000;

    if (!req.headers().empty() && req.headers().count("content-length"))
    {
        std::istringstream iss(req.headers()["content-length"]);
        size_t contentLen;
        iss >> contentLen;
        bufSize = contentLen;
    }
    buf = new char[bufSize];

    // receiving request
    std::cout << "Receiving request data: " << std::endl;
    bytesRec = recv(sockets[clientNo].fd, buf, bufSize, 0);
    if (bytesRec < 0)
        std::cout << "Failed to receive request: " << strerror(errno) << std::endl;
    else if (bytesRec == 0)
        std::cout << "Connection closed by client" << std::endl;
    if (bytesRec <= 0)
    {
        closeConnection(clientNo);
        delete[] buf;
        return;
    }
    req.appendToBuffer(buf, bytesRec);
    // std::cout << "bytes = " << bytesRec << ", msg: " << std::endl;
    // std::cout << "---------------------------------------------------------\n";
    // std::cout << buf;
    // std::cout << "---------------------------------------------------------\n";
    delete[] buf;
}

void Server::acceptNewConnection(size_t listenerNo)
{
    int newFd;
    sockaddr_storage theirAddr;
    socklen_t addrSize;

    addrSize = sizeof(theirAddr);
    newFd = accept(sockets[listenerNo].fd, (sockaddr *) &theirAddr, &addrSize);
    if (newFd == -1)
    {
        std::cout << "Accept failed" << std::endl;
        return;
    }
    fcntl(newFd, F_SETFL, O_NONBLOCK);   // enable this when doing partial recv
    std::cout << "New connection! fd = " << newFd << std::endl;
    if (sockets.size() < MAX_CLIENTS)
    {
        cons.insert(std::make_pair(newFd, Connection(sockets[listenerNo].fd)));
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
    int eventFd;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigInthandler);
    while (true)
    {
        SystemCallException::checkErr("poll", poll(&sockets[0], sockets.size(), -1));
        for (size_t i = 0; i < sockets.size(); i++)
        {
            eventFd = sockets[i].fd;
            // if one of the fds got something new to read
            if (sockets[i].revents & POLLIN)
            {
                if (configBlocks.count(eventFd))   // this fd is one of the server listeners
                    acceptNewConnection(i);
                else   // its one of the clients
                {
                    try 
                    {
                        recvData(i);
                        // if this fd is still alive and no errors so far
                        if (cons.count(eventFd))
                            // check if the headers are ready, parse them if they are
                            if (cons[eventFd].request.parseRequest())
                                readBody(i);   // this will only be run if the headers are parsed
                    }
                    catch (std::exception &e)
                    {
                        std::cerr << "not Parse error: " << e.what() << std::endl;
                        closeConnection(i);
                    }
                }
            }
            else if (sockets[i].revents & POLLOUT)
                respondToRequest(i);
        }
        if (quit)
            break;
    }
}

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
    for (size_t i = 0; i < sockets.size(); i++)
        close(sockets[i].fd);
}
