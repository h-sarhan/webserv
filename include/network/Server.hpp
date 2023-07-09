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
#include <exception>
#include <map>
#include <string>

#define MAX_CLIENTS 10

class Server
{
  private:
    std::string name;
    std::string port;
    addrinfo *servInfo;
    int listener;
    pollfd clients[MAX_CLIENTS];
    int fdCount;
    // std::map<std::string route, Location location> locations;
    // std::map<int errCode, std::string pageLocation>	errorPages;

  public:
    Server();
    Server(std::string name, std::string port);
    void bindSocket();
    void startListening();
    void handleConnection(int newFd);
    int find_empty_slot();
    ~Server();
};

#endif
