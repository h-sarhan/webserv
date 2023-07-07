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
    std::string serveDir;        // Required
    uint32_t bodySize;           // Optional
    bool listDirectories;        // Optional, false by default
    std::string directoryFile;   // Optional
    std::string cgiExtensions;   // Optional
    std::string redirectTo;      // Required if serveDir is not provided
    std::set<HTTPMethod> methodsAllowed;   // Methods allowed on this route
};

/**
 * @brief This struct holds the configuration of a single server block
 */
struct ServerBlockConfig
{
    uint32_t port;                                      // Required
    std::string hostname;                               // Optional
    std::map<uint32_t, const std::string> errorPages;   // Optional
    std::map<std::string, const RouteConfig> routes;    // At least one route
};

/**
 * @brief This class defines the overall configuration of our web server.
 * If no configuration file is provided then a default one will be created.
 */
class ServerConfig
{
  private:
    std::vector<const ServerBlockConfig> _serverBlockConfigs;

  public:
    ServerConfig(void);
    ServerConfig(const std::string &configFile);
    ~ServerConfig(void);

  private:
    ServerConfig(const ServerConfig &config);
    ServerConfig &operator=(const ServerConfig &config);
};

#endif
