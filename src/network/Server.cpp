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

// change this to use fd directly and close stuff in caller func
void Server::readBody(size_t clientNo)
{
    Request &req = cons.at(sockets[clientNo].fd).request;

    if (req.headers().count("content-length"))
    {
        std::istringstream iss(req.headers()["content-length"]);
        size_t contentLen;
        iss >> contentLen;
        if (req.requestLength() != contentLen)
            return;
        // return here so that we dont set the events to start responding
    }
    else if (req.headers().count("transfer-encoding"))
    {
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
    std::cout << std::string(req.buffer(), req.buffer() + req.requestLength());
    std::cout << "---------------------------------------------------------\n";
}

static std::string createResponse(std::string filename, std::string headers)
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

static std::string createHTMLResponse(std::string page, std::string headers)
{
    std::stringstream responseBuffer;

    responseBuffer << headers;
    responseBuffer << page.length() << "\r\n\r\n";
    responseBuffer << page;
    return responseBuffer.str();
}

void Server::sendResponse(size_t clientNo)
{
    ssize_t bytesSent;
    Connection &c = cons[sockets[clientNo].fd];

    // Request &req = cons.at(sockets[clientNo].fd).request;
    // int listener = cons.at()
    // std::string &res = cons.at(sockets[clientNo].fd).response;
    // size_t &totalSent = cons.at(sockets[clientNo].fd).totalBytesSent;

    if (c.request.requestLength() > 0)
    {
        RequestTarget target = c.request.target(configBlocks[c.listener]);
        std::cout << "resource found at: " << target.resource << std::endl;
        std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
        switch (target.type)
        {
        case FOUND:
            c.response = createResponse(target.resource, HTTP_HEADERS);
            break;
        case REDIRECTION:
            c.response = createResponse(target.resource, HTTP_HEADERS);
            break;
        case METHOD_NOT_ALLOWED:
            c.response = createHTMLResponse(errorPage(405), HTTP_HEADERS);
            break;
        case DIRECTORY:
            c.response = createHTMLResponse(directoryListing(target.resource), HTTP_HEADERS);
            break;
        case NOT_FOUND:
            c.response = createHTMLResponse(errorPage(404), HTTP_HEADERS);
            break;
        }
        c.totalBytesSent = 0;
        c.request.clear();
    }
    if (c.response.length() == 0)
    {
        std::cout << "Nothing to send >:(" << std::endl;
        return;
    }
    std::cout << "Sending a response... " << std::endl;
    bytesSent = send(sockets[clientNo].fd, c.response.c_str() + c.totalBytesSent,
                     c.response.length() - c.totalBytesSent, 0);
    if (bytesSent < 0)
        std::cout << "Sending response failed: " << strerror(errno) << std::endl;
    else
    {
        c.totalBytesSent += bytesSent;
        if (c.totalBytesSent < c.response.length())   // partial send
        {
            std::cout << "Response only sent partially: " << bytesSent
                      << ". Total: " << c.totalBytesSent << std::endl;
            return;
        }
        else   // everything got sent
        {
            std::cout << "Response sent successfully to fd " << clientNo << ", "
                      << sockets[clientNo].fd << ", total bytes sent = " << c.totalBytesSent
                      << std::endl;
            // if keep-alive was requested
            //      clear response string, set totalBytesSent to 0 and return here!!
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

void Server::recvData(size_t clientNo)
{
    char *buf = new char[REQ_BUFFER_SIZE];
    Request &req = cons.at(sockets[clientNo].fd).request;
    int bytesRec;

    // receiving request
    std::cout << "Receiving request data: " << std::endl;
    bytesRec = recv(sockets[clientNo].fd, buf, REQ_BUFFER_SIZE, 0);
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
                if (configBlocks.count(eventFd)) // this fd is one of the server listeners
                    acceptNewConnection(i);
                else   // its one of the clients
                {
                    recvData(i);
                    // if this fd is still alive and no errors so far
                    if (cons.count(eventFd))
                        // check if the headers are ready, parse them if they are
                        if (cons[eventFd].request.parseRequest())
                            readBody(i);   // this will only be run if the headers are parsed
                }
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
