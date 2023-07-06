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

static void printToken(const Token &token)
{
    std::cout << "|" << token.contents() << " "
              << Tokenizer::tokenToStr[token.type()] << "|" << std::endl;
}

Tokenizer::Tokenizer(const std::string &filename) : _line(0)
{
    std::ifstream configFile;

    // Load file
    configFile.open(filename);

    // Check if the file opened correctly
    if (!configFile)
        throw std::runtime_error("Could not open config file");

    tokenizeFile(configFile);

    // Close the file
    configFile.close();
    std::for_each(_tkns.begin(), _tkns.end(), printToken);
}

void Tokenizer::tokenizeFile(std::ifstream &configFile)
{
    std::string line;
    uint32_t lineNumber = 1;
    while (std::getline(configFile, line))
    {
        if (!configFile)
            throw std::runtime_error("Error reading config file");

        tokenizeLine(line, lineNumber);
        lineNumber++;
    }
}

void Tokenizer::tokenizeLine(std::string line, const uint32_t lineNum)
{
    // Trim out comments
    size_t poundPos = line.find('#');
    if (poundPos != std::string::npos)
        line = line.substr(0, poundPos);

    std::stringstream lineStream(line);
    tokenIterator lineStart(lineStream);

    for (tokenIterator it = lineStart; it != tokenIterator(); it++)
    {
        std::string contents = *it;
        uint32_t start;
        if (lineStream.tellg() == -1)
            start = line.length() - contents.length();
        else
            start = (uint32_t) lineStream.tellg() - contents.length();
        tokenizeStr(contents, start, lineNum);
    }
}

const std::vector<const Token> &Tokenizer::tokens(void) const
{
    return _tkns;
}

bool Tokenizer::isSingleCharToken(const char c) const
{
    if (c == '{' || c == '}' || c == '#' || c == ';')
        return true;
    return false;
}

void Tokenizer::addSingleCharToken(const char tokenChr, const uint32_t line,
                                   const uint32_t column)
{
    const std::string tokenStr(1, tokenChr);
    const Token token = Token(strToToken[tokenStr], tokenStr, line, column);
    _tkns.push_back(token);
}

void Tokenizer::addWord(uint32_t &i, const std::string &contents,
                        const uint32_t lineNum, const uint32_t column)
{
    const uint32_t wordStart = i;

    while (i < contents.length() && !isSingleCharToken(contents[i]))
        i++;

    const uint32_t wordEnd = i;
    if (i < contents.length() && isSingleCharToken(contents[i]) && i > 0)
        i--;

    const std::string tokenStr = contents.substr(wordStart, wordEnd);
    // Assume we are looking at a word
    TokenType tokenType = WORD;

    // Unless it is a keyword
    if (strToToken.count(tokenStr) != 0)
        tokenType = strToToken[tokenStr];

    const Token token(tokenType, tokenStr, lineNum, column);
    _tkns.push_back(token);
}

void Tokenizer::tokenizeStr(const std::string &contents, const uint32_t start,
                            const uint32_t lineNum)
{
    uint32_t i = 0;
    while (i < contents.length())
    {
        const char c = contents[i];
        if (c == '#')
            return;
        if (isSingleCharToken(c))
            addSingleCharToken(c, lineNum, start + i + 1);
        else
            addWord(i, contents, lineNum, start + i + 1);
        i++;
    }
}

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

std::map<const std::string, const TokenType> Tokenizer::strToToken =
    createStrToToken();

std::map<const TokenType, const std::string> Tokenizer::tokenToStr =
    createTokenToStr();
