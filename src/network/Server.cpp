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
    sockets.push_back(createPollFd(this->listener, POLLIN));
}

// change this to use fd directly and close stuff in caller func
void Server::readRequest(size_t clientNo)
{
    int bytes_rec;
    char *buf = new char[2000000];

    // receiving request
    std::cout << "Received a request: " << std::endl;
    bytes_rec = recv(sockets[clientNo].fd, buf, 2000000, 0);
    if (bytes_rec < 0)
        std::cout << "Failed to receive request" << std::endl;
    else if (bytes_rec == 0)
        std::cout << "Connection closed by client" << std::endl;
    if (bytes_rec <= 0)
    {
        close(sockets[clientNo].fd);
        cons.erase(sockets[clientNo].fd);
        sockets.erase(sockets.begin() + clientNo);
        delete[] buf;
    }
    buf[bytes_rec] = 0;
    std::cout << "bytes = " << bytes_rec << ", msg: " << std::endl;
    std::cout << "---------------------------------------------------------\n";
    std::cout << buf;
    std::cout << "---------------------------------------------------------\n";
    sockets[clientNo].events = POLLIN | POLLOUT;
    cons.at(sockets[clientNo].fd).request = buf;
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

// static int sendAll(int clientFd, const char *msg, size_t msgLen)
// {
//     ssize_t bytesLeft = msgLen;
//     ssize_t bytesSent = 0;
//     ssize_t totalSent = 0;
//     // pollfd client = createPollFd(clientFd, POLLOUT);

//     while (bytesLeft > 0)
//     {
//         // checkErr("poll", poll(&client, 1, -1));
//         bytesSent = send(clientFd, msg + totalSent, bytesLeft, 0);
//         if (bytesSent == -1)
//             return (-1);
//         bytesLeft -= bytesSent;
//         totalSent += bytesSent;
//     }
//     return (totalSent);
// }

void Server::sendResponse(size_t clientNo)
{
    std::string msg;
    size_t bytesSent;
    int fd;

    fd = sockets[clientNo].fd;
    if (cons.at(fd).request.length() > 0)
    {
        if (cons.at(fd).request.find("/artgallerycontent/2020_3.jpg") != std::string::npos)
            cons.at(fd).response = createResponse("artgallerycontent/2020_3.jpg", IMG_HEADERS);
        else
            // cons.at(fd).response = createResponse("artgallery.html", HTTP_HEADERS);
            cons.at(fd).response = createResponse("assets/bible.txt", HTTP_HEADERS);
        cons.at(fd).request.clear();
    }
    if (cons.at(fd).response.length() == 0)
    {
        std::cout << "Nothing to send >:(" << std::endl;
        return ;
    }
    msg = cons.at(fd).response;
    std::cout << "Sending a response... " << std::endl;
    bytesSent = send(fd, msg.c_str(), msg.length(), 0);
    if (bytesSent < 0)
        std::cout << "Sending response failed" << std::endl;
    else if (bytesSent < msg.length())
    {
        std::cout << "Response only sent partially: " << bytesSent << std::endl;
        cons.at(fd).response = msg.substr(bytesSent, msg.length() - bytesSent);
    }
    else // if (bytesSent == msg.length()) // everything sent completely
    {
        std::cout << "Response sent to fd " << clientNo << ", " << fd
                  << ", bytesSent = " << bytesSent << std::endl;
        close(fd);
        cons.erase(fd);
        sockets.erase(sockets.begin() + clientNo);
    }
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
    // fcntl(newFd, F_SETFL, O_NONBLOCK); // enable this when doing partial recv
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
    checkErr("listen", listen(this->listener, QUEUE_LIMIT));
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
