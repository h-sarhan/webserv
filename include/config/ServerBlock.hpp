/**
 * @file ServerBlock.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the attributes of a server block in the config file
 * @date 2023-07-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "enums/HTTPMethods.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>

/**
 * @brief This struct holds the configuration of a single route
 */
struct Route
{
    std::string serveDir;                  // Required
    size_t bodySize;                       // Optional
    bool listDirectories;                  // Optional, false by default
    std::set<std::string> cgiExtensions;   // Optional
    std::string listDirectoriesFile;       // Optional
    std::string redirectTo;                // Required if serveDir is not provided
    std::set<HTTPMethod> methodsAllowed;   // Methods allowed on this route
};

/**
 * @brief This struct holds the configuration of a single server block
 */
struct ServerBlock
{
    unsigned int port;                                // Required
    std::string hostname;                             // Optional
    std::map<unsigned int, std::string> errorPages;   // Optional
    std::map<std::string, Route> routes;              // At least one route
    static std::vector<ServerBlock> createDefaultConfig();
};

// Convenient typedef for the server config
typedef std::vector<ServerBlock> &serverList;

// Print a ServerBlock
std::ostream &operator<<(std::ostream &os, const ServerBlock &block);

// Print entire configuration
std::ostream &operator<<(std::ostream &os, const std::vector<ServerBlock> &config);

/**
 * @brief Function object to match a ServerBlock * against a HostName.
 * For use with C++ standard library algorithms
 */
struct MatchHostName
{
    MatchHostName(const std::string &hostname);
    bool operator()(const ServerBlock *serverBlock);
    bool operator()(const ServerBlock &serverBlock);

  private:
    const std::string &_hostname;
};

#endif
