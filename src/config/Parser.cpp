/**
 * @file Parser.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the config Parser class
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/Parser.hpp"
#include "config/ParseError.hpp"
#include "config/Tokenizer.hpp"
#include "config/Validators.hpp"
#include <cassert>
#include <limits>
#include <sstream>

// * Config file Grammar

// CONFIG_FILE := SERVER [SERVER]...
// SERVER := "server" { [SRV_OPTION]... LISTEN  [SRV_OPTION]...}
// LISTEN := "listen" valid_port ;
// SRV_OPTION := SERVER_NAME | ERROR_PAGE
// SERVER_NAME := "server_name" valid_hostname ;
// ERROR_PAGE := "error_page" valid_error_response valid_HTML_path ;
// LOCATION := "location" valid_URL {
//                      [LOC_OPTION]... (TRY_FILES | REDIRECT) [LOC_OPTION]...}
// TRY_FILES := "try_files" valid_dir ;
// REDIRECT := "redirect" valid_URL ;
// LOC_OPTION := BODY_SIZE | METHODS | DIR_LISTING | DIR_LISTING_FILE | CGI
// BODY_SIZE := "body_size" positive_number ;
// METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... ;
// DIR_LISTING := "directory_listing" ("true" | "false") ;
// DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path ;
// CGI := "cgi_extensions" ("php" | "python")... ;

// TODO: I use `(atEnd() || currentToken() != TOKEN)` a lot to check if the next
// TODO: token matches the grammar. I should encapsulate this into a function

HTTPMethod strToHTTPMethod(const std::string &str)
{
    if (str == "GET")
        return GET;
    if (str == "POST")
        return POST;
    if (str == "DELETE")
        return DELETE;
    if (str == "PUT")
        return PUT;
    return OTHER;
}

std::string httpMethodtoStr(HTTPMethod tkn)
{
    if (tkn == GET)
        return "GET";
    if (tkn == POST)
        return "POST";
    if (tkn == DELETE)
        return "DELETE";
    if (tkn == PUT)
        return "PUT";
    return "ERROR";
}

Parser::Parser(const std::string &configFile) : _filename(configFile)
{
    Tokenizer tokenizer(configFile);

    const std::vector<Token> &tokens = tokenizer.tokens();
    if (tokens.empty())
        throw ParseError("config file cannot be empty", _filename);

    _currToken = tokens.begin();
    _lastToken = tokens.end();
    parseConfig();
}

void Parser::parseConfig()
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
        throwParseError("expected top level `server` rule");
}

void Parser::parseServerBlock()
{
    _parsedAttributes.erase(SERVER_NAME);
    _parsedAttributes.erase(LISTEN);
    _parsedErrorPages.clear();

    _serverConfig.push_back(ServerBlock());
    _currServerBlock = _serverConfig.end() - 1;

    if (atEnd() || currentToken() != SERVER)
        throwParseError("expected top level `server` rule");
    assert(currentToken() == SERVER);
    advanceToken();

    if (atEnd() || currentToken() != LEFT_BRACE)
        throwParseError("expected '{' to start server block");
    assert(currentToken() == LEFT_BRACE);
    advanceToken();

    if (atEnd() || !atServerOption())
        throwParseError("expected a valid server option");
    assert(atServerOption());

    while (!atEnd() && atServerOption())
    {
        parseServerOption();
        advanceToken();
    }

    if (atEnd() || currentToken() != RIGHT_BRACE)
        throwParseError("unexpected token");
    assert(currentToken() == RIGHT_BRACE);

    if (_parsedAttributes.count(LISTEN) == 0)
        throwParseError("server block missing `listen` rule");

    if (_parsedAttributes.count(LOCATION) == 0)
        throwParseError("server block missing `location` block");

    if (_currServerBlock->hostname.empty())
        _currServerBlock->hostname = "localhost";
}

void Parser::parseServerOption()
{
    switch (currentToken())
    {
    case LISTEN:
        parseListenRule();
        break;
    case SERVER_NAME:
        parseServerName();
        break;
    case ERROR_PAGE:
        parseErrorPage();
        break;
    case LOCATION:
        parseLocationBlock();
        break;
    default:
        throwParseError("expected a valid server option");
    }
}

void Parser::parseListenRule()
{
    // LISTEN := "listen" valid_port SEMICOLON
    if (_parsedAttributes.count(LISTEN) != 0)
        throwParseError("duplicate `listen` rule not allowed");
    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid port number");
    assert(currentToken() == WORD);

    if (validatePort(_currToken->contents()) == false)
        throwParseError("expected a valid port number");

    std::stringstream portStream(_currToken->contents());

    portStream >> _currServerBlock->port;

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");

    // _listenSet = true;
    _parsedAttributes.insert(LISTEN);
}

void Parser::parseServerName()
{
    // SERVER_NAME := "server_name" valid_hostname SEMICOLON
    if (_parsedAttributes.count(SERVER_NAME) != 0)
        throwParseError("duplicate `server_name` rule not allowed");
    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid host name");
    assert(currentToken() == WORD);

    if (validateHostName(_currToken->contents()) == false)
        throwParseError("expected a valid host name");

    _currServerBlock->hostname = _currToken->contents();

    advanceToken();

    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");
    _parsedAttributes.insert(SERVER_NAME);
}

void Parser::parseErrorPage()
{
    // ERROR_PAGE := "error_page" valid_error_response valid_HTML_path SEMICOLON
    int response;
    std::string htmlPath;

    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a 4XX or 5XX response code");
    assert(currentToken() == WORD);

    if (validateErrorResponse(_currToken->contents()) == false)
        throwParseError("expected a 4XX or 5XX response code");

    std::stringstream responseStream(_currToken->contents());
    responseStream >> response;

    if (_parsedErrorPages.count(response) != 0)
        throwParseError("duplicate response code not allowed");
    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid path to an HTML file");
    assert(currentToken() == WORD);

    if (validateHTMLFile(_currToken->contents()) == false)
        throwParseError("expected a valid path to an HTML file");

    std::stringstream htmlStream(_currToken->contents());
    htmlStream >> htmlPath;
    _currServerBlock->errorPages.insert(std::make_pair(response, htmlPath));

    _parsedErrorPages.insert(response);
    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");
}

void Parser::parseLocationBlock()
{
    // LOCATION := "location" valid_URL LEFT_BRACE [LOC_OPTION]... (TRY_FILES | \
    // REDIRECT) [LOC_OPTION]...RIGHT_BRACE

    _parsedAttributes.erase(TRY_FILES);
    _parsedAttributes.erase(REDIRECT);
    _parsedAttributes.erase(BODY_SIZE);
    _parsedAttributes.erase(METHODS);
    _parsedAttributes.erase(DIRECTORY_TOGGLE);
    _parsedAttributes.erase(DIRECTORY_FILE);
    _parsedAttributes.erase(CGI_EXTENSION);

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected valid path");
    assert(currentToken() == WORD);

    const std::string &routePath = _currToken->contents();

    _currRoute = _currServerBlock->routes.insert(std::make_pair(routePath, Route())).first;

    advanceToken();
    if (atEnd() || currentToken() != LEFT_BRACE)
        throwParseError("expected `{` to start location block");
    assert(currentToken() == LEFT_BRACE);
    advanceToken();

    if (atEnd() || !atLocationOption())
        throwParseError("expected a valid location option");
    assert(atLocationOption());

    while (!atEnd() && atLocationOption())
    {
        parseLocationOption();
        advanceToken();
    }
    if (atEnd())
        throwParseError("unexpected end of file");
    if (currentToken() != RIGHT_BRACE)
        throwParseError("unexpected token");

    if (_parsedAttributes.count(REDIRECT) == 0 && _parsedAttributes.count(TRY_FILES) == 0)
        throwParseError("location block requires either a `try_files` or a `redirect` rule");
    _parsedAttributes.insert(LOCATION);

    if (_currRoute->second.bodySize == 0)
        _currRoute->second.bodySize = std::numeric_limits<unsigned int>::max();
    if (_currRoute->second.methodsAllowed.empty())
        _currRoute->second.methodsAllowed.insert(GET);
}

void Parser::parseLocationOption()
{
    switch (currentToken())
    {
    case TRY_FILES:
        parseTryFiles();
        break;
    case REDIRECT:
        parseRedirect();
        break;
    case BODY_SIZE:
        parseBodySize();
        break;
    case METHODS:
        parseHTTPMethods();
        break;
    case DIRECTORY_TOGGLE:
        parseDirectoryToggle();
        break;
    case DIRECTORY_FILE:
        parseDirectoryFile();
        break;
    case CGI_EXTENSION:
        parseCGI();
        break;
    default:
        throwParseError("unexpected token");
        break;
    }
}

void Parser::parseTryFiles()
{
    if (_parsedAttributes.count(TRY_FILES) != 0)
        throwParseError("multiple `try_files` rules not allowed");

    if (_parsedAttributes.count(REDIRECT) != 0)
        throwParseError("a location block cannot have both a `try_files` and a "
                        "`redirect` rule");
    // TRY_FILES := "try_files" valid_dir SEMICOLON
    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid directory");
    assert(currentToken() == WORD);

    if (validateDirectory(_currToken->contents()) == false)
        throwParseError("invalid directory");

    _currRoute->second.serveDir = _currToken->contents();

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");
    // _tryFilesSet = true;
    _parsedAttributes.insert(TRY_FILES);
}

void Parser::parseBodySize()
{
    // BODY_SIZE := "body_size" positive_number SEMICOLON
    if (_parsedAttributes.count(BODY_SIZE) != 0)
        throwParseError("multiple `body_size` rules not allowed");
    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid body size in bytes [10 - 2^32]");
    assert(currentToken() == WORD);

    if (validateBodySize(_currToken->contents()) == false)
        throwParseError("expected a valid body size in bytes [10 - 2^32]");

    std::stringstream bodySizeStream(_currToken->contents());

    bodySizeStream >> _currRoute->second.bodySize;

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");
    assert(currentToken() == SEMICOLON);

    _parsedAttributes.insert(BODY_SIZE);
}

void Parser::parseHTTPMethods()
{
    // METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... SEMICOLON
    std::set<HTTPMethod> &methods = _currRoute->second.methodsAllowed;
    if (_parsedAttributes.count(METHODS) != 0)
        throwParseError("multiple `method` rules not allowed");
    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("Expected HTTP method");
    assert(currentToken() == WORD);

    while (!atEnd() && currentToken() == WORD)
    {
        const HTTPMethod method = strToHTTPMethod(_currToken->contents());
        if (method == OTHER)
            throwParseError("invalid HTTP `method` specified");

        if (methods.count(method) != 0)
            throwParseError("duplicate method specified");

        methods.insert(method);
        advanceToken();
    }
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");

    _parsedAttributes.insert(METHODS);
}

void Parser::parseRedirect()
{
    // REDIRECT := "redirect" valid_URL SEMICOLON
    if (_parsedAttributes.count(REDIRECT) != 0)
        throwParseError("multiple `redirect` rules not allowed");
    if (_parsedAttributes.count(TRY_FILES) != 0)
        throwParseError("a location block cannot have both a `try_files` and a "
                        "`redirect` rule");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid URL");
    assert(currentToken() == WORD);

    if (validateURL(_currToken->contents()) == false)
        throwParseError("expected a valid URL");

    _currRoute->second.redirectTo = _currToken->contents();

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");

    _parsedAttributes.insert(REDIRECT);
}

void Parser::parseDirectoryToggle()
{
    // DIR_LISTING := "directory_listing" ("true" | "false") SEMICOLON
    if (_parsedAttributes.count(DIRECTORY_TOGGLE) != 0)
        throwParseError("multiple `directory_listing` rules not allowed");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a `true` or `false`");
    assert(currentToken() == WORD);

    bool &toggle = _currRoute->second.listDirectories;
    if (_currToken->contents() == "true")
        toggle = true;
    else if (_currToken->contents() == "false")
        toggle = false;
    else
        throwParseError("expected a `true` or `false`");
    advanceToken();

    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");
    assert(currentToken() == SEMICOLON);

    _parsedAttributes.insert(DIRECTORY_TOGGLE);
}

void Parser::parseDirectoryFile()
{
    // DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path SEMICOLON
    if (_parsedAttributes.count(DIRECTORY_FILE) != 0)
        throwParseError("multiple `directory_listing_file` rules not allowed");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected valid path to HTML file");
    assert(currentToken() == WORD);

    if (validateHTMLFile(_currToken->contents()) == false)
        throwParseError("expected valid path to HTML file");

    _currRoute->second.listDirectoriesFile = _currToken->contents();

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");
    assert(currentToken() == SEMICOLON);

    _parsedAttributes.insert(DIRECTORY_FILE);
}

void Parser::parseCGI()
{
    // CGI := "cgi_extensions" ("php" | "python")... SEMICOLON
    if (_parsedAttributes.count(CGI_EXTENSION) != 0)
        throwParseError("multiple `cgi_extensions` rules not allowed");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected valid cgi");
    assert(currentToken() == WORD);

    std::set<std::string> &cgis = _currRoute->second.cgiExtensions;
    while (!atEnd() && currentToken() == WORD)
    {
        const std::string cgi = _currToken->contents();
        if (cgi != "php" && cgi != "python")
            throwParseError("expected valid cgi");

        if (cgis.count(cgi) != 0)
            throwParseError("duplicate cgi specified");

        cgis.insert(cgi);
        advanceToken();
    }
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");

    _parsedAttributes.insert(CGI_EXTENSION);
}

bool Parser::atServerOption() const
{
    return (currentToken() == SERVER_NAME || currentToken() == ERROR_PAGE ||
            currentToken() == LISTEN || currentToken() == LOCATION);
}

bool Parser::atLocationOption() const
{
    return (currentToken() == TRY_FILES || currentToken() == BODY_SIZE ||
            currentToken() == METHODS || currentToken() == DIRECTORY_TOGGLE ||
            currentToken() == CGI_EXTENSION || currentToken() == REDIRECT ||
            currentToken() == DIRECTORY_FILE);
}

void Parser::throwParseError(const std::string &msg) const
{
    const Token token = atEnd() ? *(_currToken - 1) : *_currToken;
    throw ParseError(msg, token, _filename);
}

TokenType Parser::currentToken() const
{
    if (atEnd())
        throwParseError("unexpected end of file");
    return (_currToken)->type();
}

void Parser::advanceToken()
{
    _currToken++;
}

bool Parser::atEnd() const
{
    return _currToken >= _lastToken;
}

Parser::~Parser()
{
}

const std::vector<ServerBlock> &Parser::getConfig() const
{
    return _serverConfig;
}
