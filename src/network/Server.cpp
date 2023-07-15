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
#include "network/SystemCallException.hpp"

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
        // initListener(getServerBlocks(it->port, virtualServers));
        initListener(it->port, virtualServers);
    }
}

bool Server::portAlreadyInUse(unsigned int port)
{
    std::map<int, std::vector<ServerBlock *> >::iterator it;
    for (it = listeners.begin(); it != listeners.end(); it++)
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
    listeners.insert(std::make_pair(listenerFd, config));
}

// change this to use fd directly and close stuff in caller func
void Server::readRequest(size_t clientNo)
{
    size_t contentLen = 200000;   // temporary contentLen until the one from the request is parsed
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
        return;
    }
    buf[bytesRec] = 0;
    totalRec += bytesRec;
    req += buf;

    // if we're here, the call to recv was successful and we should
    // at least have received the headers (check for a \r\n\r\n)
    // at this point, the header of the request needs to be parsed to
    // determine content length / chunked encoding

    // if this is a POST req and totalRec != contentLength
    // append buf to request;
    // delete[] buf
    // return here so that we dont set the events to start responding
    std::cout << "Request size = " << totalRec << std::endl;
    totalRec = 0;   // getting ready for next request
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
        return;
    }
    std::cout << "Sending a response... " << std::endl;
    bytesSent = send(sockets[clientNo].fd, res.c_str() + totalSent, res.length() - totalSent, 0);
    if (bytesSent < 0)
        std::cout << "Sending response failed: " << strerror(errno) << std::endl;
    else
    {
        totalSent += bytesSent;
        if (totalSent < res.length())   // partial send
        {
            std::cout << "Response only sent partially: " << bytesSent << ". Total: " << totalSent
                      << std::endl;
            return;
        }
        else   // everything got sent
        {
            std::cout << "Response sent successfully to fd " << clientNo << ", "
                      << sockets[clientNo].fd << ", total bytes sent = " << totalSent << std::endl;
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
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigInthandler);
    while (true)
    {
        SystemCallException::checkErr("poll", poll(&sockets[0], sockets.size(), -1));
        for (size_t i = 0; i < sockets.size(); i++)
        {
            // server listener got something new to read
            if (sockets[i].revents & POLLIN)
            {
                // if (sockets[i].fd == listener)
                if (listeners.count(sockets[i].fd))
                    acceptNewConnection(i);
                else   // its one of the clients
                       // if we didnt receive the header for this connection yet,
                    // readHeader();
                    // ^this reads till the header (or more) and parses it to get info like
                    // Content-Length or Transfer-encoding so we can decide how to read the
                    // rest of the request
                    // else
                    readRequest(i);
                // ^this will be only to read the rest of the request according to the
                // header-specified requirements
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
    // freeaddrinfo(servInfo);
    for (size_t i = 0; i < sockets.size(); i++)
        close(sockets[i].fd);
}
