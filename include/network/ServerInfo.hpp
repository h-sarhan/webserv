/**
 * @file ServerInfo.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Wrapper class for addrInfo struct
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP

#include "SystemCallException.hpp"
#include "network.hpp"
#include "logger/Logger.hpp"

#define BACKLOG  10

using logger::log;
class ServerInfo
{
  private:
    addrinfo hints;
    addrinfo *info;

  public:
    ServerInfo();
    void getServerInfo(int port);
    int bindSocketToPort();
    ~ServerInfo();
};

#endif
