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

#define MAX_CLIENTS 10

class Server
{
  private:
    std::string name;
    std::string port;
    addrinfo *servInfo;
    int listener;
    std::vector<pollfd> clients;
    // const std::vector<ServerBlock>& virtualServers;

    // std::map<std::string route, Location location> locations;
    // std::map<int errCode, std::string pageLocation>	errorPages;

  public:
    Server();
    Server(std::string name, std::string port);
    void bindSocket();
    void startListening();
    void acceptNewConnection();
    std::string readRequest(size_t clientNo);
    void sendResponse(size_t clientNo, std::string request);
    ~Server();
};

#endif
