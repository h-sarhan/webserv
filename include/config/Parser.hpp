/**
 * @file Parser.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the config Parser class
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include "ServerBlock.hpp"
#include "Token.hpp"

/**
 * @brief This class is responsible for parsing the config file
 */
class Parser
{
  private:
    std::string _filename;
    std::vector<ServerBlock> _serverConfig;
    std::vector<Token>::const_iterator _currToken;
    std::vector<Token>::const_iterator _lastToken;
    std::vector<ServerBlock>::iterator _currServerBlock;
    std::map<std::string, Route>::iterator _currRoute;

  public:
    Parser(const std::string &fileName);
    ~Parser();

    const std::vector<ServerBlock> &getConfig() const;

  private:
    Parser(const Parser &config);
    Parser &operator=(const Parser &config);

    // Functions to check and increment current token
    TokenType currentToken() const;
    void advanceToken();
    bool atEnd() const;
    void throwParseError(const std::string &str) const;
    bool atServerOption() const;
    bool atLocationOption() const;

    // Rule parsing functions
    void parseConfig();
    void parseServerBlock();
    void parseListenRule();
    void parseServerName();
    void parseErrorPage();
    void parseServerOption();
    void parseLocationBlock();
    void parseLocationOption();
    void parseTryFiles();
    void parseBodySize();
    void parseHTTPMethods();
    void parseRedirect();
    void parseDirectoryToggle();
    void parseDirectoryFile();
    void parseCGI();

    // Kepping track of the parsed attributes
    std::set<int> _parsedAttributes;
    std::set<int> _parsedErrorPages;
};

#endif
