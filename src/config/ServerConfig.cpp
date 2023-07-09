/**
 * @file ServerConfig.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the ServerConfig class
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ServerConfig.hpp"
#include <limits>

/**
 * @brief Create a Default Route object
 *
 * @return Route
 */
static Route createDefaultRoute(void)
{
    Route defaultRoute;
    defaultRoute.serveDir = "./assets/web";
    // Max body size is unlimited by default
    defaultRoute.bodySize = std::numeric_limits<size_t>::max();
    defaultRoute.listDirectories = true;
    defaultRoute.directoryFile = "";
    defaultRoute.redirectTo = "";
    // Only GET is allowed by default
    defaultRoute.methodsAllowed.insert(GET);
    return defaultRoute;
}

/**
 * @brief Create a Default Server Block object
 *
 * @return ServerBlock
 */
static ServerBlock createDefaultServerBlock(void)
{
    ServerBlock defaultServerBlock;
    defaultServerBlock.port = 80;
    defaultServerBlock.hostname = "";
    defaultServerBlock.errorPages.insert(
        std::make_pair(404, "./assets/404.html"));
    defaultServerBlock.errorPages.insert(
        std::make_pair(502, "./assets/502.html"));
    defaultServerBlock.routes.insert(std::make_pair("/", createDefaultRoute()));
    return defaultServerBlock;
}

ServerConfig::ServerConfig(void)
{
    _serverBlocks.push_back(createDefaultServerBlock());
}

ServerConfig::~ServerConfig(void)
{
}
