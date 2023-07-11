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
#include <sstream>

/**
 * @brief Create a Default Route object
 *
 * @return Route
 */
static Route createDefaultRoute()
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
static ServerBlock createDefaultServerBlock()
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

Parser::Parser() : _configFile("")
{
    _serverBlocks.push_back(createDefaultServerBlock());
}

Parser::Parser(const std::string &configFile) : _configFile(configFile)
{
    Tokenizer tokenizer(configFile);

    const std::vector<Token> &tokens = tokenizer.tokens();
    if (tokens.empty())
        throw ParseError("config file cannot be empty", _configFile);

    _currToken = tokens.begin();
    _tokenEnd = tokens.end();
    parseConfigFile();
}

// buggy implementation use recursion instead of loops
void Parser::parseConfigFile()
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

void Parser::parseListenRule()
{
    // LISTEN := "listen" valid_port SEMICOLON
    if (_listenSet)
        throwParseError("duplicate `listen` rule not allowed");
    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid port number");
    assert(currentToken() == WORD);

    if (validatePort(_currToken->contents()) == false)
        throwParseError("expected a valid port number");

    std::stringstream portStream(_currToken->contents());

    portStream >> (_serverBlocks.end() - 1)->port;

    advanceToken();

    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");

    _listenSet = true;
}

void Parser::parseServerName()
{
    // SERVER_NAME := "server_name" valid_hostname SEMICOLON
    if (_serverNameSet)
        throwParseError("duplicate `server_name` rule not allowed");
    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid host name");
    assert(currentToken() == WORD);

    if (validateHostName(_currToken->contents()) == false)
        throwParseError("expected a valid host name");
    advanceToken();

    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");
    _serverNameSet = true;
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
    if (_errorPageSet.count(response) != 0)
        throwParseError("duplicate response code not allowed");
    _errorPageSet.insert(response);
    advanceToken();

    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid path to an HTML file");
    assert(currentToken() == WORD);
    if (validateHTMLFile(_currToken->contents()) == false)
        throwParseError("expected a valid path to an HTML file");

    std::stringstream htmlStream(_currToken->contents());
    htmlStream >> htmlPath;

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");
}

void Parser::parseTryFiles()
{
    if (_tryFilesSet)
        throwParseError("multiple `try_files` rules not allowed");
    if (_redirectSet)
        throwParseError("a location block cannot have both a `try_files` and a "
                        "`redirect` rule");
    // TRY_FILES := "try_files" valid_dir SEMICOLON
    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid directory");
    assert(currentToken() == WORD);

    if (validateDirectory(_currToken->contents()) == false)
        throwParseError("invalid directory");

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");
    _tryFilesSet = true;
}

void Parser::parseBodySize()
{
    // BODY_SIZE := "body_size" positive_number SEMICOLON
    if (_bodySizeSet)
        throwParseError("multiple `body_size` rules not allowed");
    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid body size in bytes [10 - 2^32]");
    assert(currentToken() == WORD);

    if (validateBodySize(_currToken->contents()) == false)
        throwParseError("expected a valid body size in bytes [10 - 2^32]");

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");
    assert(currentToken() == SEMICOLON);

    _bodySizeSet = true;
}

static HTTPMethod strToMethod(const std::string &str)
{
    if (str == "GET")
        return GET;
    if (str == "POST")
        return POST;
    if (str == "DELETE")
        return DELETE;
    if (str == "PUT")
        return PUT;
    return ERROR;
}

void Parser::parseHTTPMethods()
{
    // METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... SEMICOLON
    std::set<HTTPMethod> methods;
    if (_methodsSet)
        throwParseError("multiple `method` rules not allowed");
    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("Expected HTTP method");
    assert(currentToken() == WORD);

    while (!atEnd() && currentToken() == WORD)
    {
        const HTTPMethod method = strToMethod(_currToken->contents());
        if (method == ERROR)
            throwParseError("invalid HTTP `method` specified");

        if (methods.count(method) != 0)
            throwParseError("duplicate method specified");

        methods.insert(method);
        advanceToken();
    }
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");

    _methodsSet = true;
}

void Parser::parseRedirect()
{
    // REDIRECT := "redirect" valid_URL SEMICOLON
    if (_redirectSet)
        throwParseError("multiple `redirect` rules not allowed");
    if (_tryFilesSet)
        throwParseError("a location block cannot have both a `try_files` and a "
                        "`redirect` rule");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a valid URL");
    assert(currentToken() == WORD);

    if (validateURL(_currToken->contents()) == false)
        throwParseError("expected a valid URL");

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected a `;`");

    _redirectSet = true;
}

void Parser::parseDirectoryToggle()
{
    // DIR_LISTING := "directory_listing" ("true" | "false") SEMICOLON
    if (_directoryToggleSet)
        throwParseError("multiple `directory_listing` rules not allowed");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected a `true` or `false`");
    assert(currentToken() == WORD);

    bool toggle;
    (void) toggle;
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
    _directoryToggleSet = true;
}

void Parser::parseDirectoryFile()
{
    // DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path SEMICOLON
    if (_directoryFileSet)
        throwParseError("multiple `directory_listing_file` rules not allowed");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected valid path to HTML file");
    assert(currentToken() == WORD);

    if (validateHTMLFile(_currToken->contents()) == false)
        throwParseError("expected valid path to HTML file");

    advanceToken();
    if (atEnd() || currentToken() != SEMICOLON)
        throwParseError("expected `;`");
    assert(currentToken() == SEMICOLON);

    _directoryFileSet = true;
}

void Parser::parseCGI()
{
    // CGI := "cgi_extensions" ("php" | "python")... SEMICOLON
    if (_cgiSet)
        throwParseError("multiple `cgi_extensions` rules not allowed");

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected valid cgi");
    assert(currentToken() == WORD);

    std::set<std::string> cgis;
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

    _cgiSet = true;
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

void Parser::parseLocationBlock()
{
    // LOCATION := "location" valid_URL LEFT_BRACE [LOC_OPTION]... (TRY_FILES | \
    // REDIRECT) [LOC_OPTION]...RIGHT_BRACE
    _tryFilesSet = false;
    _redirectSet = false;
    _bodySizeSet = false;
    _methodsSet = false;
    _directoryToggleSet = false;
    _directoryFileSet = false;
    _cgiSet = false;

    advanceToken();
    if (atEnd() || currentToken() != WORD)
        throwParseError("expected valid URL");
    assert(currentToken() == WORD);

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
    throw ParseError(msg, token, _configFile);
}

void Parser::parseServerBlock()
{
    _serverNameSet = false;
    _listenSet = false;
    _errorPageSet.clear();

    // ? I will have to manually check if there was a listen rule
    // ? To check this I will give the port a default value of -1
    _serverBlocks.push_back(ServerBlock());
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
}

TokenType Parser::currentToken() const
{
    // ! protect this somehow
    return (_currToken)->type();
}

void Parser::advanceToken()
{
    _currToken++;
}

bool Parser::atEnd() const
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

Parser::~Parser()
{
}
