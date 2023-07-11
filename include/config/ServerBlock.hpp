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
    std::string serveDir;                     // Required
    size_t bodySize;                          // Optional
    bool listDirectories;                     // Optional, false by default
    std::string directoryFile;                // Optional
    std::vector<std::string> cgiExtensions;   // Optional
    std::string redirectTo;                   // Required if serveDir is not provided
    std::set<HTTPMethod> methodsAllowed;      // Methods allowed on this route
};

/**
 * @brief This struct holds the configuration of a single server block
 */
struct ServerBlock
{
    int port;                                               // Required
    std::string hostname;                                   // Optional
    std::map<unsigned int, const std::string> errorPages;   // Optional
    std::map<std::string, const Route> routes;              // At least one route
};

ServerBlock createDefaultServerBlock();

#endif
