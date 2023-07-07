/**
 * @file Tokenizer.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the Tokenizer class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>

/**
 * @brief Static helper function to print a token
 *
 * @param token Token to be printed
 */
static void printToken(const Token &token)
{
    std::cout << "|" << token << "|" << std::endl;
}

/**
 * @brief Construct a new Tokenizer object
 *
 * @param filename The path to the config file
 * @throws std::runtime_error Throws an exception when the configuration
 * file can't be opened
 */
ConfigTokenizer::ConfigTokenizer(const std::string &filename) : _line(0)
{
    std::ifstream configStream;

    // Load file
    configStream.open(filename);

    // Check if the file opened correctly
    if (!configStream)
        throw std::runtime_error("Could not open config file");

    tokenizeFile(configStream);

    // Printing tokens for debugging
    std::for_each(_tokens.begin(), _tokens.end(), printToken);
}

/**
 * @brief Tokenizes the configuration file
 *
 * @param configStream Input file stream of the config file
 * @throws std::runtime_error Throws an error when there is an issue reading
 * the file
 */
void ConfigTokenizer::tokenizeFile(std::ifstream &configStream)
{
    std::string lineStr;
    uint32_t lineNum = 1;
    while (std::getline(configStream, lineStr))
    {
        if (!configStream)
            throw std::runtime_error("Error reading config file");

        tokenizeLine(lineStr, lineNum);
        lineNum++;
    }
}

/**
 * @brief Splits a line into whitespace separated words and then tokenizes each
 * word
 *
 * @param lineStr The string containing the line
 * @param lineNum
 */
void ConfigTokenizer::tokenizeLine(std::string &lineStr, const uint32_t lineNum)
{
    // Trim out comments
    size_t commentPos = lineStr.find('#');
    if (commentPos != std::string::npos)
        lineStr = lineStr.substr(0, commentPos);

    std::stringstream lineStream(lineStr);
    tokenIterator wordsStart(lineStream);

    for (tokenIterator it = wordsStart; it != tokenIterator(); it++)
    {
        std::string wordStr = *it;
        uint32_t wordPos;
        if (lineStream.tellg() == -1)
            wordPos = lineStr.length() - wordStr.length();
        else
            wordPos = (uint32_t) lineStream.tellg() - wordStr.length();
        tokenizeWord(wordStr, wordPos, lineNum);
    }
}

/**
 * @brief Returns the token list generated
 *
 * @return const std::vector<Token>& The token list
 */
const std::vector<const Token> &ConfigTokenizer::tokens(void) const
{
    return _tokens;
}

/**
 * @brief Checks if a character is a token
 *
 * @param c Character that we are checking
 * @return bool Returns true if it is a token
 */
bool ConfigTokenizer::isSingleCharToken(const char c) const
{
    if (c == '{' || c == '}' || c == '#' || c == ';')
        return true;
    return false;
}

/**
 * @brief Adds a WORD token to the token list. This also checks if the word is a
 * reserved word and changes its type appropriately
 *
 * @param wordIdx The index to where we are in the wordStr
 * @param wordStr The string containing the word
 * @param lineNum The line number we are currently in
 * @param column The column we are currently in
 */
void ConfigTokenizer::addWord(uint32_t &wordIdx, const std::string &wordStr,
                              const uint32_t lineNum, const uint32_t column)
{
    const uint32_t wordStart = wordIdx;

    while (wordIdx < wordStr.length() && !isSingleCharToken(wordStr[wordIdx]))
        wordIdx++;

    const uint32_t wordEnd = wordIdx;
    if (wordIdx < wordStr.length() && isSingleCharToken(wordStr[wordIdx]) &&
        wordIdx > 0)
        wordIdx--;

    const std::string tokenStr = wordStr.substr(wordStart, wordEnd);

    // Check if the token is a reserved keyword
    TokenType type;
    if (strToToken.count(tokenStr) != 0)
        type = strToToken[tokenStr];
    else
        type = WORD;

    _tokens.push_back(Token(type, tokenStr, lineNum, column));
}

/**
 * @brief Goes through a whitespace separated word and extracts tokens from it
 *
 * @param wordStr The string containing the whitespace separated word
 * @param wordPos Where the word is in the line
 * @param lineNum The line number we are in
 */
void ConfigTokenizer::tokenizeWord(const std::string &wordStr,
                                   const uint32_t wordPos,
                                   const uint32_t lineNum)
{
    uint32_t wordIdx = 0;
    while (wordIdx < wordStr.length())
    {
        const char c = wordStr[wordIdx];
        if (c == '#')
            return;
        uint32_t column = wordPos + wordIdx + 1;
        if (isSingleCharToken(c))
        {
            const std::string tokenStr(1, c);
            const TokenType type = strToToken[tokenStr];
            const Token token(type, tokenStr, lineNum, column);
            _tokens.push_back(token);
        }
        else
            addWord(wordIdx, wordStr, lineNum, column);
        wordIdx++;
    }
}

/**
 * @brief Helper function to generate the strToToken map
 *
 */
static const std::map<const std::string, const TokenType> createStrToToken(void)
{
    std::map<const std::string, const TokenType> tokenMap;
    tokenMap.insert(std::make_pair("server", SERVER));
    tokenMap.insert(std::make_pair("listen", LISTEN));
    tokenMap.insert(std::make_pair("server_name", SERVER_NAME));
    tokenMap.insert(std::make_pair("error_page", ERROR_PAGE));
    tokenMap.insert(std::make_pair("methods", METHODS));
    tokenMap.insert(std::make_pair("directory_listing", DIRECTORY_TOGGLE));
    tokenMap.insert(std::make_pair("directory_listing_file", DIRECTORY_FILE));
    tokenMap.insert(std::make_pair("cgi_extensions", CGI_EXTENSION));
    tokenMap.insert(std::make_pair("redirect", REDIRECT));
    tokenMap.insert(std::make_pair("{", LEFT_BRACE));
    tokenMap.insert(std::make_pair("}", RIGHT_BRACE));
    tokenMap.insert(std::make_pair("#", POUND));
    tokenMap.insert(std::make_pair(";", SEMICOLON));
    return tokenMap;
}

/**
 * @brief Helper function to generate the tokenToStr map
 *
 */
static const std::map<const TokenType, const std::string> createTokenToStr(void)
{
    std::map<const TokenType, const std::string> tokenMap;
    tokenMap.insert(std::make_pair(SERVER, "SERVER"));
    tokenMap.insert(std::make_pair(LISTEN, "LISTEN"));
    tokenMap.insert(std::make_pair(SERVER_NAME, "SERVER_NAME"));
    tokenMap.insert(std::make_pair(ERROR_PAGE, "ERROR_PAFE"));
    tokenMap.insert(std::make_pair(METHODS, "METHODS"));
    tokenMap.insert(std::make_pair(DIRECTORY_TOGGLE, "DIRECTORY_LISTING"));
    tokenMap.insert(std::make_pair(DIRECTORY_FILE, "DIRECTORY_LISTING_FILE"));
    tokenMap.insert(std::make_pair(CGI_EXTENSION, "CGI_EXTENSIONS"));
    tokenMap.insert(std::make_pair(REDIRECT, "REDIRECT"));
    tokenMap.insert(std::make_pair(LEFT_BRACE, "LEFT_BRACE"));
    tokenMap.insert(std::make_pair(RIGHT_BRACE, "RIGHT_BRACE"));
    tokenMap.insert(std::make_pair(POUND, "POUND"));
    tokenMap.insert(std::make_pair(SEMICOLON, "SEMICOLON"));
    tokenMap.insert(std::make_pair(WORD, "WORD"));
    return tokenMap;
}

/**
 * @brief Destroy the Config Tokenizer object
 */
ConfigTokenizer::~ConfigTokenizer(void)
{
}

// Assigning the static maps
std::map<const std::string, const TokenType> ConfigTokenizer::strToToken =
    createStrToToken();
std::map<const TokenType, const std::string> ConfigTokenizer::tokenToStr =
    createTokenToStr();
