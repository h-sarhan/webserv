/**
 * @file ServerConfig.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the ServerConfig class and the ServerBlock and Route
 * config structs
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
struct Route
{
    std::string serveDir;                     // Required
    size_t bodySize;                          // Optional
    bool listDirectories;                     // Optional, false by default
    std::string directoryFile;                // Optional
    std::vector<std::string> cgiExtensions;   // Optional
    std::string redirectTo;   // Required if serveDir is not provided
    std::set<HTTPMethod> methodsAllowed;   // Methods allowed on this route
};

/**
 * @brief This struct holds the configuration of a single server block
 */
struct ServerBlock
{
    unsigned int port;                                      // Required
    std::string hostname;                                   // Optional
    std::map<unsigned int, const std::string> errorPages;   // Optional
    std::map<std::string, const Route> routes;   // At least one route
};

/**
 * @brief This class defines the overall configuration of our web server.
 * If no configuration file is provided then a default one will be created.
 */
class ServerConfig
{
  private:
    std::vector<ServerBlock> _serverBlocks;

  public:
    ServerConfig(void);
    ServerConfig(const std::string &configFile);
    ~ServerConfig(void);

  private:
    ServerConfig(const ServerConfig &config);
    ServerConfig &operator=(const ServerConfig &config);

    bool validatePort(const std::string &portStr) const;
    bool validateURL(const std::string &urlStr) const;
    void validationTests(void) const;
    bool validateDirectory(const std::string &dirPath) const;
    bool validateHTMLFile(const std::string &htmlFile) const;
    bool validateErrorResponse(const std::string &response) const;
    bool validateHostName(const std::string &hostname) const;
};

#endif
