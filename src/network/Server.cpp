/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 20:51:26 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/22 20:51:26 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include "enums/ResourceTypes.hpp"
#include "enums/conversions.hpp"
#include "network/SystemCallException.hpp"
#include "network/network.hpp"
#include "responses/Response.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

bool quit = false;

std::map<int, std::vector<ServerBlock *> > Server::configBlocks =
    std::map<int, std::vector<ServerBlock *> >();

Server::Server(serverList virtualServers)
{
    std::cout << "Virtual servers - " << std::endl;
    std::cout << virtualServers << std::endl;
    std::vector<ServerBlock>::iterator it;
    for (it = virtualServers.begin(); it != virtualServers.end(); it++)
    {
        if (portAlreadyInUse(it->port))
            continue;
        Log(INFO) << "Setting up listener on port " << it->port << std::endl;
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

std::vector<ServerBlock *> &Server::getConfig(int listener)
{
    // if listener key doesnt exist in this map it will create a default vector but the caller
    // should check for it
    return configBlocks[listener];
}

void Server::closeConnection(int clientNo)
{
    Log(INFO) << "Closing connection " << sockets[clientNo].fd << std::endl;
    close(sockets[clientNo].fd);
    cons.erase(sockets[clientNo].fd);
    sockets.erase(sockets.begin() + clientNo);
}

void Server::respondToRequest(size_t clientNo)
{
    Connection &c = cons.at(sockets[clientNo].fd);
    int sendStatus;

    c.processRequest();
    sendStatus = c.response().sendResponse(sockets[clientNo].fd);
    if (sendStatus == IDLE_CONNECTION)
    {
        time_t curTime;
        time(&curTime);
        if (!c.keepAlive())
            return;
        else if (curTime - c.startTime() >= c.timeOut())
        {
            Log(WARN) << "Connection " << sockets[clientNo].fd << " timed out! (idle for " << c.timeOut()
                      << "s): " << sockets[clientNo].fd << std::endl;
            closeConnection(clientNo);
        }
        return;
    }
    if (sendStatus == SEND_PARTIAL)
        return;
    if (sendStatus == SEND_SUCCESS)
        if (c.keepConnectionAlive())
        {
            Log(INFO) << "Connection " << sockets[clientNo].fd << " is keep alive" << std::endl;
            return;
        }
    closeConnection(clientNo);
}

static void sigInthandler(int sigNo)
{
    (void) sigNo;
    quit = true;
}

// change this to use fd directly and close stuff in caller func
void Server::readBody(size_t clientNo)
{
    Request &req = cons.at(sockets[clientNo].fd).request();
    
    if (req.usesContentLength())
    {
        if (!req.contentLenReached())
            return ;
    }
    else if (req.usesChunkedEncoding())
    {
        if (!req.chunkedEncodingComplete())
            return ;
    }
    Log(SUCCESS) << "Request recieved from connection " << sockets[clientNo].fd << ". Size = " << req.length() << std::endl;
    Log(DBUG) << enumToStr(req.method()) << " " << req.resource().originalRequest << std::endl;
}

void Server::recvData(size_t clientNo)
{
    Request &req = cons.at(sockets[clientNo].fd).request();
    char *buf;
    ssize_t bytesRec;
    size_t bufSize = READ_SIZE;

    if (!req.headers().empty() && req.headers().count("content-length"))
        bufSize = fromStr<size_t>(req.headers().at("content-length"));
    buf = new char[bufSize];

    Log(INFO) << "Receiving request data from connection " << sockets[clientNo].fd << "... " << std::endl;
    bytesRec = recv(sockets[clientNo].fd, buf, bufSize, 0);
    if (bytesRec < 0)
        Log(ERR) << "Failed to receive request from connection" << sockets[clientNo].fd << ": " << strerror(errno) << std::endl;
    else if (bytesRec == 0)
        Log(ERR) << "Connection " << sockets[clientNo].fd << " closed by client" << std::endl;
    if (bytesRec <= 0)
    {
        closeConnection(clientNo);
        delete[] buf;
        return;
    }
    req.appendToBuffer(buf, bytesRec);
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
        Log(ERR) << "Accept failed" << std::endl;
        return;
    }
    fcntl(newFd, F_SETFL, O_NONBLOCK);   // enable this when doing partial recv
    char ipBuf[INET_ADDRSTRLEN];
    std::string ip(inet_ntop(AF_INET, &theirAddr, ipBuf, INET_ADDRSTRLEN), INET_ADDRSTRLEN);
    cons.insert(std::make_pair(newFd, Connection(sockets[listenerNo].fd, ip)));
    sockets.push_back(createPollFd(newFd, POLLIN | POLLOUT));
    Log(SUCCESS) << "New connection! Socket: " << newFd << ", IP: " << ip << std::endl;
    if (sockets.size() - configBlocks.size() > MAX_CLIENTS)
    {
        cons.at(newFd).dropped() = true;
        cons.at(newFd).response().createHTMLResponse(503, errorPage(503), false);
        Log(ERR) << "Maximum clients reached, dropping this connection" << std::endl;
    }
}

void Server::startListening()
{
    int eventFd;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigInthandler);
    while (!quit)
    {
        SystemCallException::checkErr("poll", poll(&sockets[0], sockets.size(), -1));
        for (size_t i = 0; i < sockets.size(); i++)
        {
            eventFd = sockets[i].fd;
            if ((sockets[i].revents & POLLOUT) && cons.at(eventFd).dropped())
                respondToRequest(i);
            // if one of the fds got something new to read
            else if (sockets[i].revents & POLLIN)
            {
                if (configBlocks.count(eventFd))   // this fd is one of the server listeners
                    acceptNewConnection(i);
                else   // its one of the clients
                {
                    recvData(i);
                    // if this fd is still alive and no errors so far
                    if (cons.count(eventFd))
                        // check if the headers are ready, parse them if they are
                        if (cons[eventFd].request().parseRequest())
                            readBody(i);   // this will only be run if the headers are parsed
                }
            }
            else if ((sockets[i].revents & POLLOUT))
                respondToRequest(i);

        }
    }
}

Server::~Server()
{
    Log(SUCCESS) << "Server destructor called" << std::endl;
    for (size_t i = 0; i < sockets.size(); i++)
        close(sockets[i].fd);
}
