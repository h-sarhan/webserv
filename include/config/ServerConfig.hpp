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

#include "Token.hpp"
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
    int port;                                               // Required
    std::string hostname;                                   // Optional
    std::map<unsigned int, const std::string> errorPages;   // Optional
    std::map<std::string, const Route> routes;   // At least one route
};

// TODO: Refactor this to be a ConfigParser class
/**
 * @brief This class defines the overall configuration of our web server.
 * If no configuration file is provided then a default one will be created.
 */
class ServerConfig
{
  private:
    std::vector<ServerBlock> _serverBlocks;
    std::vector<Token>::const_iterator _currToken;
    std::vector<Token>::const_iterator _tokenEnd;
    const std::string _configFile;

  public:
    ServerConfig(void);
    ServerConfig(const std::string &configFile);
    ~ServerConfig(void);

  private:
    ServerConfig(const ServerConfig &config);
    ServerConfig &operator=(const ServerConfig &config);

    TokenType currentToken(void) const;
    void advanceToken(void);
    bool atEnd(void) const;
    void throwParseError(const std::string &str) const;
    bool atServerOption(void) const;
    bool atLocationOption(void) const;

    // Rule parsing functions
    void parseServerName(void);
    void parseErrorPage(void);
    void parseLocationBlock(void);
    void parseConfigFile(void);
    void parseServerBlock(void);
    void parseListenRule(void);
    void parseServerOption(void);
    void parseLocationOption(void);
    void parseTryFiles(void);
    void parseBodySize(void);
    void parseHTTPMethods(void);

    bool _serverNameSet;
    bool _listenSet;
    std::set<int> _errorPageSet;
    bool _tryFilesSet;
    bool _redirectSet;
    bool _bodySizeSet;
    bool _methodsSet;
};

#endif
