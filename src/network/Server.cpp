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
    // clients = new pollfd[MAX_CLIENTS];
    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].fd = -1;
    fdCount = 0;
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
    if ((status = getaddrinfo(NULL, port.c_str(), &hints, &servInfo)) !=
        0)   // first param is host IP/name and its null because we set ai_flags
             // as AI_PASSIVE
        throw SystemCallException("getaddrinfo", gai_strerror(status));
    // clients = new pollfd[MAX_CLIENTS];
    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].fd = -1;
    fdCount = 0;
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
    clients[0].fd = this->listener;
    clients[0].events = POLLIN | POLL_OUT;
    fdCount++;
}

// change this to use fd directly and close stuff in caller func
bool Server::readRequest(int fdNum)
{
    int bytes_rec;
    char *buf = new char[2000];

    // receiving request
    std::cout << "Received a request: " << std::endl;
    bytes_rec = recv(clients[fdNum].fd, buf, 2000, 0);
    if (bytes_rec < 0)
        std::cout << "Failed to receive request" << std::endl;
    else if (bytes_rec == 0)
        std::cout << "Connection closed by client" << std::endl;
    if (bytes_rec <= 0)
    {
        close(clients[fdNum].fd);
        clients[fdNum].fd = -1;   // poll will ignore this element now
        clients[fdNum].events = 0;
        clients[fdNum].revents = 0;
        fdCount--;
        delete[] buf;
        return (false);
    }
    buf[bytes_rec] = 0;
    std::cout << "bytes = " << bytes_rec << ", msg: " << std::endl;
    std::cout << "---------------------------------------------------------\n";
    std::cout << buf << std::endl;
    std::cout << "---------------------------------------------------------\n";
    // close(clients[fdNum].fd);
    // clients[fdNum].fd = -1;   // poll will ignore this element now
    // fdCount--;
    delete[] buf;
    return (true);
}

void Server::sendResponse(int fdNum)
{
    std::string msg = HW_HTML;                    
    std::cout << "Sending a response: " << std::endl;
    if (send(clients[fdNum].fd, msg.c_str(), msg.length(), 0) < 0)
        std::cout << "Sending response failed" << std::endl;
    else
        std::cout << "Response sent!" << std::endl;
    close(clients[fdNum].fd);
    clients[fdNum].fd = -1;
    clients[fdNum].events = 0;
    clients[fdNum].revents = 0;
    fdCount--;
}

static void sig_int_handler(int sigNo)
{
    (void) sigNo;
    quit = true;
}

int Server::find_empty_slot()
{
    int i;
    for (i = 0; i < MAX_CLIENTS && clients[i].fd > 0; i++)
        ;
    return (i);
}

void Server::startListening()
{
    int newFd;
    int slot;
    int pollCount;
    sockaddr_storage their_addr;
    socklen_t addr_size;

    signal(SIGINT, sig_int_handler);
    checkErr("listen", listen(this->listener, QUEUE_LIMIT));
    addr_size = sizeof(their_addr);
    std::cout << "Listening on port " << this->port << std::endl;
    while (true)
    {
        // -1 timeout means wait forever
        std::cout << "Waiting for a request" << std::endl;
        pollCount = checkErr("poll", poll(clients, fdCount, -1));
        for (int i = 0; i < fdCount; i++)
        {
            if (clients[i].revents & POLLIN && clients[i].fd == listener)   // server listener got something new to read
            {
                // new incoming connection
                std::cout << "New connection!" << std::endl;
                // if accept fails here we dont necessarily have to exit, we
                // could continue; the loop
                newFd = checkErr("accept", accept(this->listener,
                                                    (sockaddr *) &their_addr,
                                                    &addr_size));
                if (fdCount < MAX_CLIENTS)
                {
                    slot = find_empty_slot();
                    std::cout << "empty slot = " << slot << std::endl;
                    clients[slot].fd = newFd;
                    clients[slot].events = POLLIN | POLLOUT;
                    fdCount++;
                }
                else
                {
                    std::cout << "Maximum clients reached, dropping this "
                                    "connection"
                                << std::endl;
                    close(newFd);
                }
            }
            else if ((clients[i].revents & POLLIN) && (clients[i].revents & POLLOUT))  // one of the clients is ready to be read from
            {
                if (!readRequest(i))
                    continue;
                sendResponse(i);
            }
            else if (clients[i].revents & POLLERR) 
            {
                std::cout << "Socket error" << std::endl;
                close(clients[i].fd);
                clients[i].fd = -1;
                clients[i].events = 0;
                fdCount--;
            }
        }
    }
}

/*void Server::startListening()
{
    int newFd;
    int slot;
    int pollCount;
    sockaddr_storage their_addr;
    socklen_t addr_size;
    int temp;

    signal(SIGINT, sig_int_handler);
    checkErr("listen", listen(this->listener, QUEUE_LIMIT));
    addr_size = sizeof(their_addr);
    std::cout << "Listening on port " << this->port << std::endl;
    while (true)
    {
        // -1 timeout means wait forever
        std::cout << "Waiting for a request" << std::endl;
        pollCount = checkErr("poll", poll(clients, fdCount, -1));
        for (int i = 0; i < fdCount; i++)
        {
            if (i > 0) {
                std::cout << "++++++++++++ events is " << clients[i].events << "++++++++++++" << std::endl; 
                std::cout << "++++++++++++ revents is " << clients[i].revents << "++++++++++++" << std::endl; 
            }
            if (clients[i].revents & POLLIN)
            {
                if (clients[i].fd ==
                    listener)   // server listener got something new to read
                {
                    // new incoming connection
                    std::cout << "New connection!" << std::endl;
                    // if accept fails here we dont necessarily have to exit, we
                    // could continue; the loop
                    newFd = checkErr("accept", accept(this->listener,
                                                      (sockaddr *) &their_addr,
                                                      &addr_size));
                    if (fdCount < MAX_CLIENTS)
                    {
                        slot = find_empty_slot();
                        std::cout << "empty slot = " << slot << std::endl;
                        clients[slot].fd = newFd;
                        clients[slot].events = POLLIN | POLLOUT;
                        fdCount++;
                    }
                    else
                    {
                        std::cout << "Maximum clients reached, dropping this "
                                     "connection"
                                  << std::endl;
                        close(newFd);
                    }
                }
                else   // one of the clients is ready to be read from
                {
                    std::cout << "Received a request: " << std::endl;
                    readRequest(i);
                    clients[i].events |= POLLOUT;
                }
            }
            else if (clients[i].revents & POLLOUT)
            {
                std::cout << "socket " << clients[i].fd << " is ready to be written to" << std::endl;
                if (i == 0)
                {
                    std::cout << "this is the server" << std::endl;
                    continue;
                }
                std::string msg =
                    "HTTP/1.1 200 OK\r\nContent-Type: text/html; "
                    "charset=UTF-8\r\nContent-Length: 100\r\n\r\n<!DOCTYPE "
                    "html><html><head><title>Hello World</title></head><body><h1>Hello "
                    "World</h1></body></html>";
                std::cout << "Sending a response: " << std::endl;
                checkErr("send", temp = send(clients[i].fd, msg.c_str(), msg.length(), 0));
                std::cout << "bytes = " << temp << ", msg len = " << msg.length()
                        << std::endl
                        << std::endl;
                close(clients[i].fd);
                clients[i].fd = -1;
                clients[i].events = 0;
                clients[i].revents = 0;
                fdCount--;
            }
            else if (clients[i].revents & POLLERR) 
            {
                std::cout << "Socket error" << std::endl;
                close(clients[i].fd);
                clients[i].fd = -1;
                clients[i].events = 0;
                fdCount--;
            }
           
                        
            // else if (quit)
            // {
            //     std::cout << "ctrl c" << std::endl;
            //     break;
            // }
        }
    }
}*/

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
    freeaddrinfo(servInfo);
    // close all open connection fds
    // delete[] clients;
    if (listener != -1)
        close(listener);
}
