/**
 * @file Parser.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the ServerConfig class and the ServerBlock and Route
 * config structs
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include "Token.hpp"
#include "enums/HTTPMethods.hpp"
#include <map>
#include <set>
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
class Parser
{
  private:
    std::vector<ServerBlock> _serverBlocks;
    std::vector<Token>::const_iterator _currToken;
    std::vector<Token>::const_iterator _tokenEnd;
    const std::string _configFile;

  public:
    Parser();
    Parser(const std::string &configFile);
    ~Parser();

  private:
    Parser(const Parser &config);
    Parser &operator=(const Parser &config);

    TokenType currentToken() const;
    void advanceToken();
    bool atEnd() const;
    void throwParseError(const std::string &str) const;
    bool atServerOption() const;
    bool atLocationOption() const;

    // Rule parsing functions
    void parseServerName();
    void parseErrorPage();
    void parseLocationBlock();
    void parseConfigFile();
    void parseServerBlock();
    void parseListenRule();
    void parseServerOption();
    void parseLocationOption();
    void parseTryFiles();
    void parseBodySize();
    void parseHTTPMethods();
    void parseRedirect();
    void parseDirectoryToggle();
    void parseDirectoryFile();
    void parseCGI();

    bool _serverNameSet;
    bool _listenSet;
    std::set<int> _errorPageSet;
    bool _tryFilesSet;
    bool _redirectSet;
    bool _bodySizeSet;
    bool _methodsSet;
    bool _directoryToggleSet;
    bool _directoryFileSet;
    bool _cgiSet;
};

#endif
