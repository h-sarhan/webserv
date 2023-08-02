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
#include "logger/Logger.hpp"
#include "network.hpp"
#include "network/ServerInfo.hpp"
#include "responses/DefaultPages.hpp"

#define MAX_CLIENTS     170
#define READ_SIZE       1000000
#define START_POS(x, y) (x <= y ? 0 : x - y)

typedef std::vector<ServerBlock> &serverList;

using logger::Log;
class Server
{
  private:
    static std::map<int, std::vector<ServerBlock *> > configBlocks;
    std::vector<pollfd> sockets;
    std::map<int, Connection> cons;   // maps a socket fd to its connection data

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
    static std::vector<ServerBlock *> &getConfig(int listener);
    ~Server();
};

#endif
