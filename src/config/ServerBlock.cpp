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
#include "enums/conversions.hpp"
#include "utils.hpp"
#include <limits>
#include <sstream>

/**
 * @brief Create a default Route object for use when no config file is provided
 *
 * @return Route Default route
 */
static Route createDefaultRoute()
{
    Route defaultRoute;
    defaultRoute.serveDir = "./assets";

    // Max body size is ~4GB by default
    defaultRoute.bodySize = std::numeric_limits<unsigned int>::max();

    // Directory listing is on by default
    defaultRoute.listDirectories = true;
    defaultRoute.listDirectoriesFile = "";

    // Only GET is allowed by default
    defaultRoute.methodsAllowed.insert(GET);
    return defaultRoute;
}

/**
 * @brief Create a default ServerBlock object for use when no config file is provided
 *
 * @return ServerBlock Default server block
 */
std::vector<ServerBlock> ServerBlock::createDefaultConfig()
{
    ServerBlock defaultServerBlock;

    // Listen on localhost:3000
    defaultServerBlock.port = 3000;
    defaultServerBlock.hostname = "localhost";

    // There is only one route on "/"
    defaultServerBlock.routes.insert(std::make_pair("/", createDefaultRoute()));
    return std::vector<ServerBlock>(1, defaultServerBlock);
}

static void printErrorPage(std::string &str, const std::pair<unsigned int, std::string> &errorPage)
{
    str += "\tError response: \n";
    str += "\t\tResponse code: " + toStr(errorPage.first) + "\n";
    str += "\t\tResponse page: " + errorPage.second + "\n\n";
}

static void printRoute(std::string &str, const std::pair<std::string, Route> &route)
{
    str += "\tRoute: \n";
    str += "\t\tPath: " + route.first + "\n";
    if (!route.second.serveDir.empty())
        str += "\t\tServing directory: " + route.second.serveDir + "\n";
    if (!route.second.redirectTo.empty())
        str += "\t\tRedirect URL: " + route.second.redirectTo + "\n";
    str += "\t\tList directory?: ";
    route.second.listDirectories ? str += "yes\n" : str += "no\n";
    "\t\tList directory file: " + route.second.listDirectoriesFile + "\n";
    str += "\t\tMax body size: " + toStr(route.second.bodySize) + "\n";
    str += "\t\tMethods allowed: ";
    for (std::set<HTTPMethod>::const_iterator it = route.second.methodsAllowed.begin();
         it != route.second.methodsAllowed.end(); it++)
        str += enumToStr(*it) + " ";
    str += "\n";
    str += "\t\tCGI extensions: ";
    if (route.second.cgiExtensions.empty())
        str += "none\n";
    for (std::set<std::string>::const_iterator it = route.second.cgiExtensions.begin();
         it != route.second.cgiExtensions.end(); it++)
        str += *it + " ";
    str += "\n";
}

std::ostream &operator<<(std::ostream &os, const ServerBlock &block)
{
    std::string str;
    str += "Server config: \n\tListening on: " + toStr(block.port) + "\n";

    if (block.hostname.length() > 0)
        str += "\tHostname: " + block.hostname + "\n";

    for (std::map<unsigned int, std::string>::const_iterator it = block.errorPages.begin();
         it != block.errorPages.end(); it++)
        printErrorPage(str, *it);

    for (std::map<std::string, Route>::const_iterator it = block.routes.begin();
         it != block.routes.end(); it++)
        printRoute(str, *it);

    return os << str;
}

std::ostream &operator<<(std::ostream &os, const std::vector<ServerBlock> &config)
{
    for (std::vector<ServerBlock>::const_iterator it = config.begin(); it != config.end(); it++)
        os << *it;
    return os;
}

MatchHostName::MatchHostName(const std::string &hostname) : _hostname(hostname)
{
}

bool MatchHostName::operator()(const ServerBlock *serverBlock)
{
    return serverBlock && _hostname == serverBlock->hostname;
}

bool MatchHostName::operator()(const ServerBlock &serverBlock)
{
    return _hostname == serverBlock.hostname;
}
