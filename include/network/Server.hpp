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
#include "responses/DefaultPages.hpp"
#include "logger/Logger.hpp"

#define MAX_CLIENTS 20
#define READ_SIZE 1000000
#define POS(x, y) (y <= x ? 0 : y - x)

typedef std::vector<ServerBlock> &serverList;

class Server : Logger
{
  private:
    static std::map<int, std::vector<ServerBlock *> > configBlocks;
    std::vector<pollfd> sockets;
    std::map<int, Connection> cons;   // maps a socket fd to its connection data
    using Logger::log;

    bool portAlreadyInUse(unsigned int port);
    void initListener(unsigned int port, serverList virtualServers);
    void acceptNewConnection(size_t listenerNo);
    void closeConnection(int clientNo);
    void recvData(size_t clientNo);
    void readBody(size_t clientNo);
    void respondToRequest(size_t clientNo);
  
  public:
    Server(serverList virtualServers);
    void startListening();
    static std::vector<ServerBlock *>& getConfig(int listener);
    ~Server();
};

#endif
