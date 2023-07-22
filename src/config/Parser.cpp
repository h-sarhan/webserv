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
// LOC_OPTION := BODY_SIZE | METHODS | AUTO_INDEX | INDEX | CGI
// BODY_SIZE := "body_size" positive_number ;
// METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT" | "HEAD")... ;
// AUTO_INDEX := "autoindex" ("true" | "false") ;
// INDEX := "index" filename ;
// CGI := "cgi_extensions" ("php" | "python")... ;

/**
 * @brief Construct a new Parser object with the config file it will parse
 *
 * @param configFile
 */
Parser::Parser(const std::string &configFile) : _filename(configFile)
{
    const Tokenizer tokenizer(configFile);

    const std::vector<Token> &tokens = tokenizer.tokens();
    assertThat(!tokens.empty(), EMPTY_CONFIG);

    _currToken = tokens.begin();
    _lastToken = tokens.end();
    parseConfig();
}

/**
 * @brief Parse the configuration file
 *
 */
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

/**
 * @brief Assert that the current token is of a specific type, otherwise throw an exception
 *
 * @param type Token type to check
 * @param errMsg Error message to throw
 */
void Parser::matchToken(const TokenType type, const std::string &errMsg) const
{
    assertThat(!atEnd() && currentToken() == type, errMsg);
}

/**
 * @brief Assert that the condition is true, if not throw an exception
 *
 * @param condition Condition to check
 * @param errMsg Error message to throw
 */
void Parser::assertThat(bool condition, const std::string &errMsg) const
{
    if (!condition)
        throwParseError(errMsg);
}

/**
 * @brief Reset the parsed attributes of a server block
 */
void Parser::resetServerBlockAttributes()
{
    _parsedAttributes.erase(SERVER_NAME);
    _parsedAttributes.erase(LISTEN);
    _parsedErrorPages.clear();
}

/**
 * @brief Parse a server block from the config file
 *
 */
void Parser::parseServerBlock()
{
    resetServerBlockAttributes();

    // Push an empty server block and get an iterator to it
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

/**
 * @brief Parse a server block option: One of `listen`, `server_name`, `error_page`, or `location`
 */
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

/**
 * @brief Parse the `listen` rule
 */
void Parser::parseListenRule()
{
    // LISTEN := "listen" valid_port SEMICOLON

    assertThat(_parsedAttributes.count(LISTEN) == 0, DUPLICATE("listen"));
    advanceToken();

    matchToken(WORD, INVALID("port"));

    assertThat(validatePort(_currToken->contents()), INVALID("port"));

    _currServerBlock->port = fromStr<unsigned int>(_currToken->contents());

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(LISTEN);
}

/**
 * @brief Parse the `server_name` rule
 */
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

/**
 * @brief Parse the `error_page` rule
 */
void Parser::parseErrorPage()
{
    // ERROR_PAGE := "error_page" valid_error_response valid_HTML_path SEMICOLON
    advanceToken();

    matchToken(WORD, INVALID_ERROR_RESPONSE);

    assertThat(validateErrorResponse(_currToken->contents()), INVALID_ERROR_RESPONSE);

    const int response = fromStr<int>(_currToken->contents());

    assertThat(_parsedErrorPages.count(response) == 0, DUPLICATE("error_reponse"));
    advanceToken();

    matchToken(WORD, INVALID("HTML file"));

    assertThat(validateHTMLFile(_currToken->contents()), INVALID("HTML file"));

    _currServerBlock->errorPages.insert(std::make_pair(response, _currToken->contents()));

    _parsedErrorPages.insert(response);
    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);
}

/**
 * @brief Reset parsed location block attributes
 */
void Parser::resetLocationBlockAttributes()
{
    _parsedAttributes.erase(TRY_FILES);
    _parsedAttributes.erase(REDIRECT);
    _parsedAttributes.erase(BODY_SIZE);
    _parsedAttributes.erase(METHODS);
    _parsedAttributes.erase(AUTO_INDEX);
    _parsedAttributes.erase(INDEX);
    _parsedAttributes.erase(CGI_EXTENSION);
}

/**
 * @brief parse a `location` block
 */
void Parser::parseLocationBlock()
{
    // LOCATION := "location" valid_URL LEFT_BRACE [LOC_OPTION]... (TRY_FILES | \
    // REDIRECT) [LOC_OPTION]...RIGHT_BRACE

    resetLocationBlockAttributes();

    advanceToken();
    matchToken(WORD, INVALID("path"));

    std::string routePath = _currToken->contents();

    // Trim '/' from route path
    if (routePath != "/")
        rightTrimStr(routePath, "/");

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

/**
 * @brief Parse a `location` option. One of: `try_files` `redirect` `body_size` `methods`
 *                                           `auto_index` `index` `cgi_extension`
 */
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
    case AUTO_INDEX:
        parseAutoIndex();
        break;
    case INDEX:
        parseIndex();
        break;
    case CGI_EXTENSION:
        parseCGI();
        break;
    default:
        throwParseError("unexpected token");
        break;
    }
}

/**
 * @brief Parse the `try_files` rule
 */
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

/**
 * @brief Parse the `body_size` rule
 */
void Parser::parseBodySize()
{
    // BODY_SIZE := "body_size" positive_number SEMICOLON
    assertThat(_parsedAttributes.count(BODY_SIZE) == 0, DUPLICATE("body_size"));

    advanceToken();
    matchToken(WORD, INVALID("body size [10 - 2^32]"));

    assertThat(validateBodySize(_currToken->contents()), INVALID("body size [10 - 2^32]"));

    _currRoute->second.bodySize = fromStr<size_t>(_currToken->contents());

    advanceToken();
    matchToken(SEMICOLON, "expected `;`");

    _parsedAttributes.insert(BODY_SIZE);
}

/**
 * @brief Parse the `methods` rule
 */
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

/**
 * @brief Parse the `redirect` rule
 */
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

/**
 * @brief Parse the `autoindex` rule
 */
void Parser::parseAutoIndex()
{
    // AUTO_INDEX := "autoindex" ("true" | "false") SEMICOLON
    assertThat(_parsedAttributes.count(AUTO_INDEX) == 0, DUPLICATE("autoindex"));

    advanceToken();
    matchToken(WORD, INVALID("bool. `true` or `false`"));

    assertThat(_currToken->contents() == "true" || _currToken->contents() == "false",
               INVALID("bool. `true` or `false`"));

    bool &toggle = _currRoute->second.autoIndex;
    if (_currToken->contents() == "true")
        toggle = true;
    else
        toggle = false;

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(AUTO_INDEX);
}

/**
 * @brief Parse the `index` rule
 */
void Parser::parseIndex()
{
    // INDEX := "index" filename SEMICOLON
    assertThat(_parsedAttributes.count(INDEX) == 0, DUPLICATE("index"));

    advanceToken();
    matchToken(WORD, INVALID("path to an file"));

    _currRoute->second.indexFile = _currToken->contents();

    advanceToken();
    matchToken(SEMICOLON, EXPECTED_SEMICOLON);

    _parsedAttributes.insert(INDEX);
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

/**
 * @brief Will check the current token and determine if it is a `server` option
 *
 * @return true if the current token is a `server' option
 */
bool Parser::atServerOption() const
{
    if (atEnd())
        return false;
    switch (currentToken())
    {
    case SERVER_NAME:
    case ERROR_PAGE:
    case LISTEN:
    case LOCATION:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Will check the current token and determine if it is a `location` option
 *
 * @return true if the current token is a `location` option
 */
bool Parser::atLocationOption() const
{
    if (atEnd())
        return false;
    switch (currentToken())
    {
    case TRY_FILES:
    case BODY_SIZE:
    case METHODS:
    case AUTO_INDEX:
    case CGI_EXTENSION:
    case REDIRECT:
    case INDEX:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Throw a parse error using the invalid token. Goes to the previous token if it is the last
 * one
 * @param msg
 */
void Parser::throwParseError(const std::string &msg) const
{
    const Token &token = atEnd() ? *(_currToken - 1) : *_currToken;
    throw ParseError(msg, token, _filename);
}

/**
 * @brief Get the current token type. Throws an exception if an attempt to access an invalid token
 * was made
 *
 * @return TokenType Current token type
 */
TokenType Parser::currentToken() const
{
    if (atEnd())
        throwParseError("unexpected end of file");
    return _currToken->type();
}

/**
 * @brief Go to the next token
 */
void Parser::advanceToken()
{
    _currToken++;
}

/**
 * @brief Determine if we have reached the end of the token list
 *
 * @return true if we are at the end of the token list
 */
bool Parser::atEnd() const
{
    return _currToken >= _lastToken;
}

/**
 * @brief Destroy the Parser object
 *
 */
Parser::~Parser()
{
}

/**
 * @brief Get the list of ServerBlocks that make up our configuration
 *
 * @return std::vector<ServerBlock>& The list of ServerBlocks produced from the config file
 */
std::vector<ServerBlock> &Parser::getConfig()
{
    return _serverConfig;
}
