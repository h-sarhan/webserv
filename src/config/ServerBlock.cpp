/**
 * @file ServerBlock.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements methods to create default Server blocks
 * @date 2023-07-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/ServerBlock.hpp"

/**
 * @brief Create a Default Route object
 *
 * @return Route
 */
static Route createDefaultRoute()
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
ServerBlock createDefaultServerBlock()
{
    ServerBlock defaultServerBlock;
    defaultServerBlock.port = 80;
    defaultServerBlock.hostname = "";
    defaultServerBlock.errorPages.insert(std::make_pair(404, "./assets/404.html"));
    defaultServerBlock.errorPages.insert(std::make_pair(502, "./assets/502.html"));
    defaultServerBlock.routes.insert(std::make_pair("/", createDefaultRoute()));
    return defaultServerBlock;
}
