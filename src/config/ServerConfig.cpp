/**
 * @file ServerConfig.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the ServerConfig class
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/ServerConfig.hpp"
#include "InputValidators.hpp"
#include "config/ConfigParseError.hpp"
#include "config/Tokenizer.hpp"
#include <limits>

// ! TODO Create peek() and atEnd() functions for refactoring

/**
 * @brief Create a Default Route object
 *
 * @return Route
 */
static Route createDefaultRoute(void)
{
    Route defaultRoute;
    defaultRoute.serveDir = "./assets/web";
    // Max body size is unlimited by default
    defaultRoute.bodySize = std::numeric_limits<size_t>::max();
    defaultRoute.listDirectories = true;
    defaultRoute.directoryFile = "";
    defaultRoute.redirectTo = "";
    // Only GET is allowed by default
    defaultRoute.methodsAllowed.insert(GET);
    return defaultRoute;
}

/**
 * @brief Create a Default Server Block object
 *
 * @return ServerBlock
 */
static ServerBlock createDefaultServerBlock(void)
{
    ServerBlock defaultServerBlock;
    defaultServerBlock.port = 80;
    defaultServerBlock.hostname = "";
    defaultServerBlock.errorPages.insert(
        std::make_pair(404, "./assets/404.html"));
    defaultServerBlock.errorPages.insert(
        std::make_pair(502, "./assets/502.html"));
    defaultServerBlock.routes.insert(std::make_pair("/", createDefaultRoute()));
    return defaultServerBlock;
}

ServerConfig::ServerConfig(void) : _configFile("")
{
    _serverBlocks.push_back(createDefaultServerBlock());
}

ServerConfig::ServerConfig(const std::string &configFile)
    : _configFile(configFile)
{
    ConfigTokenizer tokenizer(configFile);

    const std::vector<Token> &tokens = tokenizer.tokens();
    if (tokens.empty())
        throw ConfigParseError("config file cannot be empty", _configFile);

    _currToken = tokens.begin();
    _tokenEnd = tokens.end();
    parseConfigFile();
}

// buggy implementation use recursion instead of loops
void ServerConfig::parseConfigFile(void)
{
    // * CONFIG_FILE := SERVER [SERVER]...
    parseServerBlock();
    assert(currentToken() == RIGHT_BRACE);
    advanceToken();
    while (!atEnd() && currentToken() == SERVER)
    {
        parseServerBlock();
        assert(currentToken() == RIGHT_BRACE);
        advanceToken();
    }
    if (!atEnd())
        parseError("expected top level `server` rule");

    // if (_currToken != _tokenEnd)
    //     throw ConfigParseError(serverBlockError, *_currToken, _configFile);
}

void ServerConfig::parseListenRule(void)
{
    // LISTEN := "listen" valid_port SEMICOLON
}

void ServerConfig::parseError(const std::string &msg)
{
    const Token token = atEnd() ? *--_currToken : *_currToken;
    throw ConfigParseError(msg, token, _configFile);
}

void ServerConfig::parseServerBlock(void)
{
    // * SERVER := "server" LEFT_BRACE [SRV_OPTION]... LISTEN  [SRV_OPTION]...\
    // LOCATION [SRV_OPTION]... RIGHT_BRACE
    if (atEnd() || currentToken() != SERVER)
        parseError("expected top level `server` rule");
    assert(currentToken() == SERVER);
    advanceToken();

    if (atEnd() || currentToken() != LEFT_BRACE)
        parseError("expected '{' to start server block");
    assert(currentToken() == LEFT_BRACE);
    advanceToken();

    if (atEnd() || currentToken() != RIGHT_BRACE)
        parseError("expected '}' to close server block");
    assert(currentToken() == RIGHT_BRACE);
}

TokenType ServerConfig::currentToken(void) const
{
    // ! protect this somehow
    return (_currToken)->type();
}

void ServerConfig::advanceToken(void)
{
    _currToken++;
}

bool ServerConfig::atEnd(void) const
{
    return _currToken >= _tokenEnd;
}

// * Config file Grammar
// * CONFIG_FILE := SERVER [SERVER]...

// SERVER := "server" LEFT_BRACE [SRV_OPTION]... LISTEN  [SRV_OPTION]...\

// LISTEN := "listen" valid_port SEMICOLON
// SRV_OPTION := SERVER_NAME | ERROR_PAGE
// SERVER_NAME := "server_name" valid_hostname SEMICOLON
// ERROR_PAGE := "error_page" valid_error_response valid_HTML_path SEMICOLON
// SEMICOLON := ";"
// LEFT_BRACE := "{"
// RIGHT_BRACE := "}"

// LOCATION := "location" valid_URL LEFT_BRACE [LOC_OPTION]... (TRY_FILES | \
// REDIRECT) [LOC_OPTION]...RIGHT_BRACE

// TRY_FILES := "try_files" valid_dir SEMICOLON
// REDIRECT := "redirect" valid_URL SEMICOLON

// LOC_OPTION := BODY_SIZE | METHODS | DIR_LISTING | DIR_LISTING_FILE | CGI

// BODY_SIZE := "body_size" positive_number SEMICOLON
// METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... SEMICOLON
// DIR_LISTING := "directory_listing" ("true" | "false") SEMICOLON

// DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path SEMICOLON
// CGI := "cgi_extensions" ("php" | "python")... SEMICOLON

ServerConfig::~ServerConfig(void)
{
}
