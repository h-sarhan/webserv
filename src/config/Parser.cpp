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
#include "enums/conversions.hpp"
#include "utils.hpp"
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
// LOCATION := "location" valid_URL { [LOC_OPTION]... (TRY_FILES | REDIRECT) [LOC_OPTION]...}
// TRY_FILES := "try_files" valid_dir ;
// REDIRECT := "redirect" valid_URL ;
// LOC_OPTION := BODY_SIZE | METHODS | DIR_LISTING | DIR_LISTING_FILE | CGI
// BODY_SIZE := "body_size" positive_number ;
// METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... ;
// DIR_LISTING := "directory_listing" ("true" | "false") ;
// DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path ;
// CGI := "cgi_extensions" ("php" | "python")... ;

Parser::Parser(const std::string &configFile) : _filename(configFile)
{
    const Tokenizer tokenizer(configFile);

    const std::vector<Token> &tokens = tokenizer.tokens();
    assertThat(!tokens.empty(), EMPTY_CONFIG);

    _currToken = tokens.begin();
    _lastToken = tokens.end();
    parseConfig();
}

void Parser::parseConfig()
{
    // * CONFIG_FILE := SERVER [SERVER]...
    parseServerBlock();
    advanceToken();
    while (!atEnd() && currentToken() == SERVER)
    {
        parseServerBlock();
        advanceToken();
    }
    assertThat(atEnd(), EXPECTED_SERVER);
}

void Parser::matchToken(const TokenType token, const std::string &errMsg) const
{
    if (atEnd() || currentToken() != token)
        throwParseError(errMsg);
}

void Parser::assertThat(bool condition, const std::string &throwMsg) const
{
    if (!condition)
        throwParseError(throwMsg);
}

void Parser::resetServerBlockAttributes()
{
    _parsedAttributes.erase(SERVER_NAME);
    _parsedAttributes.erase(LISTEN);
    _parsedErrorPages.clear();
}

void Parser::parseServerBlock()
{
    resetServerBlockAttributes();

    _serverConfig.push_back(ServerBlock());
    _currServerBlock = _serverConfig.end() - 1;

    // Set default value for localhost
    _currServerBlock->hostname = "localhost";

    matchToken(SERVER, EXPECTED_SERVER);

    advanceToken();
    matchToken(LEFT_BRACE, EXPECTED_BLOCK_START("server"));

    advanceToken();
    assertThat(atServerOption(), EXPECTED_OPTION("server"));

    while (!atEnd() && atServerOption())
    {
        parseServerOption();
        advanceToken();
    }

    matchToken(RIGHT_BRACE, EXPECTED_BLOCK_END("server"));

    assertThat(_parsedAttributes.count(LISTEN) != 0, SERVER_MISSING("listen"));
    assertThat(_parsedAttributes.count(LOCATION) != 0, SERVER_MISSING("location"));
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
        throwParseError(EXPECTED_OPTION("server"));
    }
}

void Parser::parseListenRule()
{
    // LISTEN := "listen" valid_port SEMICOLON
    assertThat(_parsedAttributes.count(LISTEN) == 0, DUPLICATE("listen"));
    advanceToken();

    matchToken(WORD, INVALID("port"));

    assertThat(validatePort(_currToken->contents()), INVALID("port"));

    fromStr(_currToken->contents(), _currServerBlock->port);

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(LISTEN);
}

void Parser::parseServerName()
{
    // SERVER_NAME := "server_name" valid_hostname SEMICOLON
    assertThat(_parsedAttributes.count(SERVER_NAME) == 0, DUPLICATE("server_name"));
    advanceToken();

    matchToken(WORD, INVALID("hostname"));

    assertThat(validateHostName(_currToken->contents()), INVALID("hostname"));

    _currServerBlock->hostname = _currToken->contents();

    advanceToken();

    matchToken(SEMICOLON, EXPECTED_SEMICOLON);
    _parsedAttributes.insert(SERVER_NAME);
}

void Parser::parseErrorPage()
{
    // ERROR_PAGE := "error_page" valid_error_response valid_HTML_path SEMICOLON
    advanceToken();

    matchToken(WORD, INVALID_ERROR_RESPONSE);

    assertThat(validateErrorResponse(_currToken->contents()), INVALID_ERROR_RESPONSE);

    int response;
    fromStr(_currToken->contents(), response);

    assertThat(_parsedErrorPages.count(response) == 0, DUPLICATE("error_reponse"));
    advanceToken();

    matchToken(WORD, INVALID("HTML file"));

    assertThat(validateHTMLFile(_currToken->contents()), INVALID("HTML file"));

    _currServerBlock->errorPages.insert(std::make_pair(response, _currToken->contents()));

    _parsedErrorPages.insert(response);
    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);
}

void Parser::resetLocationBlockAttributes()
{
    _parsedAttributes.erase(TRY_FILES);
    _parsedAttributes.erase(REDIRECT);
    _parsedAttributes.erase(BODY_SIZE);
    _parsedAttributes.erase(METHODS);
    _parsedAttributes.erase(DIRECTORY_TOGGLE);
    _parsedAttributes.erase(DIRECTORY_FILE);
    _parsedAttributes.erase(CGI_EXTENSION);
}

void Parser::parseLocationBlock()
{
    // LOCATION := "location" valid_URL LEFT_BRACE [LOC_OPTION]... (TRY_FILES | \
    // REDIRECT) [LOC_OPTION]...RIGHT_BRACE

    resetLocationBlockAttributes();

    advanceToken();
    matchToken(WORD, INVALID("path"));

    std::string routePath = _currToken->contents();

    // Trim '/' from route path
    // ! Could be refactored
    // ! Create trim functions
    if (routePath != "/" && *--routePath.end() == '/')
        routePath = routePath.substr(0, routePath.length() - 1);

    // Insert an empty route block
    _currRoute = _currServerBlock->routes.insert(std::make_pair(routePath, Route())).first;

    // Set default values
    _currRoute->second.bodySize = std::numeric_limits<unsigned int>::max();
    _currRoute->second.methodsAllowed.insert(GET);

    advanceToken();
    matchToken(LEFT_BRACE, EXPECTED_BLOCK_START("location"));

    advanceToken();
    assertThat(atLocationOption(), INVALID("location option"));

    while (!atEnd() && atLocationOption())
    {
        parseLocationOption();
        advanceToken();
    }

    assertThat(!atEnd(), UNEXPECTED_EOF);

    matchToken(RIGHT_BRACE, EXPECTED_BLOCK_END("location"));

    assertThat(_parsedAttributes.count(REDIRECT) != 0 || _parsedAttributes.count(TRY_FILES) != 0,
               MISSING_LOCATION_OPTION);

    _parsedAttributes.insert(LOCATION);
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
    // TRY_FILES := "try_files" valid_dir SEMICOLON
    assertThat(_parsedAttributes.count(TRY_FILES) == 0, DUPLICATE("try_files"));

    assertThat(_parsedAttributes.count(REDIRECT) == 0, ADDITIONAL_LOCATION_OPTION);

    advanceToken();
    matchToken(WORD, INVALID("directory"));

    assertThat(validateDirectory(_currToken->contents()), INVALID("directory"));

    _currRoute->second.serveDir = _currToken->contents();

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);
    _parsedAttributes.insert(TRY_FILES);
}

void Parser::parseBodySize()
{
    // BODY_SIZE := "body_size" positive_number SEMICOLON
    assertThat(_parsedAttributes.count(BODY_SIZE) == 0, DUPLICATE("body_size"));

    advanceToken();
    matchToken(WORD, INVALID("body size [10 - 2^32]"));

    assertThat(validateBodySize(_currToken->contents()), INVALID("body size [10 - 2^32]"));

    fromStr(_currToken->contents(), _currRoute->second.bodySize);

    advanceToken();
    matchToken(SEMICOLON, "expected `;`");

    _parsedAttributes.insert(BODY_SIZE);
}

void Parser::parseHTTPMethods()
{
    // METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... SEMICOLON
    std::set<HTTPMethod> &methods = _currRoute->second.methodsAllowed;
    methods.clear();

    assertThat(_parsedAttributes.count(METHODS) == 0, DUPLICATE("method"));

    advanceToken();
    matchToken(WORD, INVALID("method"));

    while (!atEnd() && currentToken() == WORD)
    {
        const HTTPMethod method = strToEnum<HTTPMethod>(_currToken->contents());

        assertThat(method != OTHER, INVALID("method"));
        assertThat(methods.count(method) == 0, DUPLICATE_METHOD);

        methods.insert(method);
        advanceToken();
    }

    matchToken(SEMICOLON, EXPECTED_SEMICOLON);
    _parsedAttributes.insert(METHODS);
}

void Parser::parseRedirect()
{
    // REDIRECT := "redirect" valid_URL SEMICOLON
    assertThat(_parsedAttributes.count(REDIRECT) == 0, DUPLICATE("redirect"));
    assertThat(_parsedAttributes.count(TRY_FILES) == 0, ADDITIONAL_LOCATION_OPTION);

    advanceToken();
    matchToken(WORD, INVALID("URL"));

    assertThat(validateURL(_currToken->contents()) == true, INVALID("URL"));

    _currRoute->second.redirectTo = _currToken->contents();

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(REDIRECT);
}

void Parser::parseDirectoryToggle()
{
    // DIR_LISTING := "directory_listing" ("true" | "false") SEMICOLON
    assertThat(_parsedAttributes.count(DIRECTORY_TOGGLE) == 0, DUPLICATE("directory_listing"));

    advanceToken();
    matchToken(WORD, INVALID("bool. `true` or `false`"));

    assertThat(_currToken->contents() == "true" || _currToken->contents() == "false",
               INVALID("bool. `true` or `false`"));

    bool &toggle = _currRoute->second.listDirectories;
    if (_currToken->contents() == "true")
        toggle = true;
    else
        toggle = false;

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(DIRECTORY_TOGGLE);
}

void Parser::parseDirectoryFile()
{
    // DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path SEMICOLON
    assertThat(_parsedAttributes.count(DIRECTORY_FILE) == 0, DUPLICATE("directory_listing_file"));

    advanceToken();
    matchToken(WORD, INVALID("path to an HTML file"));

    assertThat(validateHTMLFile(_currToken->contents()) == true, INVALID("path to an HTML file"));

    _currRoute->second.listDirectoriesFile = _currToken->contents();

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(DIRECTORY_FILE);
}

// ! This will need changing once we start working on CGI
void Parser::parseCGI()
{
    // CGI := "cgi_extensions" ("php" | "python")... SEMICOLON
    if (_parsedAttributes.count(CGI_EXTENSION) != 0)
        throwParseError(DUPLICATE("cgi_extensions"));

    advanceToken();
    matchToken(WORD, INVALID("cgi extension"));

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
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(CGI_EXTENSION);
}

bool Parser::atServerOption() const
{
    if (atEnd())
        return false;
    return (currentToken() == SERVER_NAME || currentToken() == ERROR_PAGE ||
            currentToken() == LISTEN || currentToken() == LOCATION);
}

bool Parser::atLocationOption() const
{
    if (atEnd())
        return false;
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
    return _currToken->type();
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

std::vector<ServerBlock> &Parser::getConfig()
{
    return _serverConfig;
}
