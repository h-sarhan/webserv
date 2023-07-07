/**
 * @file ServerConfig.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the Config class
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "common.hpp"
#include "enums/HTTPMethods.hpp"

/**
 * @brief This struct holds the configuration of a single route
 */
struct RouteConfig
{
    const std::string serveDir;        // Required
    const uint32_t bodySize;           // Optional
    const bool listDirectories;        // Optional, false by default
    const std::string directoryFile;   // Optional
    const std::string cgiExtensions;   // Optional
    const std::string redirectTo;      // Required if serveDir is not provided
    const std::set<const HTTPMethod>
        methodsAllowed;   // Methods allowed on this route
};

/**
 * @brief This struct holds the configuration of a single server block
 */
struct VirtualServerConfig
{
    const uint32_t port;                                            // Required
    const std::string hostname;                                     // Optional
    const std::map<const uint32_t, const std::string> errorPages;   // Optional
    const std::map<const std::string, const RouteConfig>
        routes;   // At least one route is required
};

/**
 * @brief This class defines the overall configuration of our web server.
 * If no configuration file is provided then a default one will be created
 */
class ServerConfig
{
  public:
    ServerConfig(void);
    ServerConfig(const std::string &configFile);
    ~ServerConfig(void);

  private:
    ServerConfig(const ServerConfig &config);
    ServerConfig &operator=(const ServerConfig &config);
};

#endif
