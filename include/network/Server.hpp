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
// #include "Connection.hpp"

#define MAX_CLIENTS 10

struct Connection
{
    std::string request;
    std::string response;
    size_t totalBytesSent;
};

class Server
{
  private:
    std::string name;
    std::string port;
    addrinfo *servInfo;
    int listener;
    std::vector<pollfd> sockets;
    std::map<int, Connection> cons;
    // const std::vector<ServerBlock>& virtualServers;

    // std::map<std::string route, Location location> locations;
    // std::map<int errCode, std::string pageLocation>	errorPages;

  public:
    Server();
    Server(std::string name, std::string port);
    void bindSocket();
    void startListening();
    void acceptNewConnection();
    void readRequest(size_t clientNo);
    void sendResponse(size_t clientNo);
    ~Server();
};

#endif
