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
#include <sstream>

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
    defaultRoute.listDirectoriesFile = "";
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
const ServerBlock createDefaultServerBlock()
{
    ServerBlock defaultServerBlock;
    defaultServerBlock.port = 80;
    defaultServerBlock.hostname = "";
    defaultServerBlock.errorPages.insert(std::make_pair(404, "./assets/404.html"));
    defaultServerBlock.errorPages.insert(std::make_pair(502, "./assets/502.html"));
    defaultServerBlock.routes.insert(std::make_pair("/", createDefaultRoute()));
    return defaultServerBlock;
}

static void printErrorPage(std::stringstream &ss,
                           const std::pair<unsigned int, std::string> &errorPage)
{
    ss << "\tError response: \n";
    ss << "\t\tResponse code: " << errorPage.first << "\n";
    ss << "\t\tResponse page: " << errorPage.second << "\n\n";
}

static void printRoute(std::stringstream &ss, const std::pair<std::string, Route> &route)
{
    ss << "\tRoute: \n";
    ss << "\t\tPath: " << route.first << "\n";
    if (!route.second.serveDir.empty())
        ss << "\t\tServing directory: " << route.second.serveDir << "\n";
    if (!route.second.redirectTo.empty())
        ss << "\t\tRedirect URL: " << route.second.redirectTo << "\n";
    ss << "\t\tList directory?: " << (route.second.listDirectories ? "yes" : "no") << "\n";
    if (!route.second.listDirectoriesFile.empty())
        ss << "\t\tList directory file: " << route.second.listDirectoriesFile << "\n";
    ss << "\t\tMax body size: " << route.second.bodySize << "\n";
    ss << "\t\tMethods allowed: ";
    for (std::set<HTTPMethod>::const_iterator it = route.second.methodsAllowed.begin();
         it != route.second.methodsAllowed.end(); it++)
        ss << httpMethodtoStr(*it) << " ";
    ss << "\n";
    ss << "\t\tCGI extensions: ";
    if (route.second.cgiExtensions.empty())
        ss << "none\n";
    for (std::set<std::string>::const_iterator it = route.second.cgiExtensions.begin();
         it != route.second.cgiExtensions.end(); it++)
        ss << *it << " ";
    ss << "\n";
}

std::ostream &operator<<(std::ostream &os, const ServerBlock &block)
{
    std::stringstream ss;
    ss << "Server config: \n";
    ss << "\tListening on: " << block.port << "\n";

    if (!block.hostname.empty())
        ss << "\tHostname: " << block.hostname << "\n";

    for (std::map<unsigned int, std::string>::const_iterator it = block.errorPages.begin();
         it != block.errorPages.end(); it++)
        printErrorPage(ss, *it);

    for (std::map<std::string, Route>::const_iterator it = block.routes.begin();
         it != block.routes.end(); it++)
        printRoute(ss, *it);

    return os << ss.str();
}

std::ostream &operator<<(std::ostream &os, const std::vector<ServerBlock> &config)
{
    for (std::vector<ServerBlock>::const_iterator it = config.begin(); it != config.end(); it++)
        os << *it;
    return os;
}
