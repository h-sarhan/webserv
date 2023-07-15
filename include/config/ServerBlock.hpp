/**
 * @file ServerBlock.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the attributes of a server block
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
    std::string listDirectoriesFile;       // Optional
    std::set<std::string> cgiExtensions;   // Optional
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
};

typedef std::vector<ServerBlock> &serverList;

ServerBlock createDefaultServerBlock();

// Print ServerBlock
std::ostream &operator<<(std::ostream &os, const ServerBlock &block);

// Print entire configuration
std::ostream &operator<<(std::ostream &os, const std::vector<ServerBlock> &config);

#endif
