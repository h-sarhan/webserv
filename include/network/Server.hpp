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

#include "Connection.hpp"
#include "SystemCallException.hpp"
#include "config/ServerBlock.hpp"
#include "network.hpp"
#include "network/ServerInfo.hpp"

#define MAX_CLIENTS 10

// struct Connection
// {
//     std::string request;
//     std::string response;
//     size_t totalBytesRec;
//     size_t totalBytesSent;
// };

typedef std::vector<ServerBlock> &serverList;

class Server
{
  private:
    std::vector<pollfd> sockets;
    std::map<int, Connection> cons;   // maps a socket fd to its connection data
    std::map<int, std::vector<ServerBlock *> > listeners;

    bool portAlreadyInUse(int port);
    void initListener(int port, serverList virtualServers);
    void acceptNewConnection(size_t listenerNo);
    void readRequest(size_t clientNo);
    void sendResponse(size_t clientNo);

  public:
    Server(serverList virtualServers);
    void startListening();
    ~Server();
};

#endif
