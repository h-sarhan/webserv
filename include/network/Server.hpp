/**
 * @file Server.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Class that describes a server instance adhering to the given
 * configuration
 * @date 2023-07-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "SystemCallException.hpp"
#include "network.hpp"
#include "Connection.hpp"
#include "config/ServerBlock.hpp"

#define MAX_CLIENTS 10

// struct Connection
// {
//     std::string request;
//     std::string response;
//     size_t totalBytesRec;
//     size_t totalBytesSent;
// };

typedef const std::vector<ServerBlock>& serverList;

class Server
{
  private:
    std::string port;
    addrinfo *servInfo;
    int listener;
    std::vector<pollfd> sockets;
    std::map<int, Connection> cons; // maps a socket fd to its connection data
    serverList virtualServers;

    // std::map<std::string route, Location location> locations;
    // std::map<int errCode, std::string pageLocation>	errorPages;

  public:
    // Server();
    Server(std::string port, serverList virtualServers);
    void bindSocket();
    void startListening();
    void acceptNewConnection();
    void readRequest(size_t clientNo);
    void sendResponse(size_t clientNo);
    ~Server();
};

#endif
