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

// Parse error messages
#define EMPTY_CONFIG                "config file cannot be empty"
#define EXPECTED_SERVER             "expected top level `server` rule"
#define EXPECTED_LEFT_BRACE         "expected `{` to "
#define EXPECTED_RIGHT_BRACE        "expected `}` to "
#define EXPECTED_BLOCK_START(BLOCK) EXPECTED_LEFT_BRACE "start `" BLOCK "` block"
#define EXPECTED_BLOCK_END(BLOCK)   EXPECTED_RIGHT_BRACE "end `" BLOCK "` block"
#define EXPECTED_OPTION(BLOCK)      "expected a valid `" BLOCK "` option"
#define SERVER_MISSING(RULE)        "server block missing `" RULE "` rule"
#define DUPLICATE(RULE)             "duplicate `" RULE "` rule not allowed"
#define INVALID(VAL)                "expected a valid " VAL
#define EXPECTED_SEMICOLON          "expected a `;`"
#define INVALID_ERROR_RESPONSE      "expected a 4XX or 5XX response code"
#define UNEXPECTED_EOF              "unexpected end of file"
#define DUPLICATE_METHOD            "duplicate HTTP method specified"
#define MISSING_LOCATION_OPTION     "location block requires either a `try_files` or a `redirect` rule"
#define ADDITIONAL_LOCATION_OPTION                                                                 \
    "a location block cannot have both a `try_files` and a "                                       \
    "`redirect` rule"

/**
 * @brief This class is responsible for parsing the config file
 */
class Parser
{
  private:
    const std::string &_filename;
    std::vector<ServerBlock> _serverConfig;
    std::vector<Token>::const_iterator _currToken;
    std::vector<Token>::const_iterator _lastToken;
    std::vector<ServerBlock>::iterator _currServerBlock;
    std::map<std::string, Route>::iterator _currRoute;
    std::set<int> _parsedAttributes;   // Parsed attributes so far
    std::set<int> _parsedErrorPages;   // Parsed error pages so far

  public:
    // Constructs a Parser object with the filename
    Parser(const std::string &fileName);
    ~Parser();

    // Gets the parsed configuration as a vector of server blocks
    std::vector<ServerBlock> &getConfig();

  private:
    // Copy constructors have been made private because this class will never be copied
    Parser(const Parser &config);
    Parser &operator=(const Parser &config);

    // Methods to check and increment current token
    TokenType currentToken() const;
    bool atEnd() const;
    bool atServerOption() const;
    bool atLocationOption() const;
    void advanceToken();

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
    void parseDirectoryListing();
    void parseIndex();
    void parseCGI();

    // Methods to reset parsed attributes
    void resetServerBlockAttributes();
    void resetLocationBlockAttributes();

    // Methods to throw errors and assert conditions
    void assertThat(bool condition, const std::string &throwMsg) const;
    void matchToken(const TokenType token, const std::string &throwMsg) const;
    void throwParseError(const std::string &str) const;
};

#endif
