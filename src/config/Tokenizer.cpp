/**
 * @file Tokenizer.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the Tokenizer class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

// ! USE ITERATORS INSTEAD OF ARRAY INDICES
#include "config/Tokenizer.hpp"
#include "enums/conversions.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * @brief Construct a new Tokenizer object
 *
 * @param filename The path to the config file
 * @throws std::runtime_error Throws an exception when the configuration
 * file can't be opened
 */
Tokenizer::Tokenizer(const std::string &filename) : _line(0)
{
    std::ifstream configStream;

    // Load file
    configStream.open(filename.c_str());

    // Check if the file opened correctly
    if (!configStream)
        throw std::runtime_error("Could not open config file");

    tokenizeFile(configStream);
}

/**
 * @brief Tokenizes the configuration file
 *
 * @param configStream Input file stream of the config file
 * @throws std::runtime_error Throws an error when there is an issue reading
 * the file
 */
void Tokenizer::tokenizeFile(std::ifstream &configStream)
{
    std::string lineStr;
    unsigned int lineNum = 1;
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
void Tokenizer::tokenizeLine(std::string &lineStr, const unsigned int lineNum)
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
        unsigned int wordPos;
        if (lineStream.tellg() == -1)
            wordPos = lineStr.length() - wordStr.length();
        else
            wordPos = (unsigned int) lineStream.tellg() - wordStr.length();
        tokenizeWord(wordStr, wordPos, lineNum);
    }
}

/**
 * @brief Returns the token list generated
 *
 * @return const std::vector<Token>& The token list
 */
const std::vector<Token> &Tokenizer::tokens() const
{
    return _tokens;
}

/**
 * @brief Checks if a character is a token
 *
 * @param c Character that we are checking
 * @return bool Returns true if it is a token
 */
bool Tokenizer::isSingleCharToken(const char c) const
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
void Tokenizer::addWord(unsigned int &wordIdx, const std::string &wordStr,
                        const unsigned int lineNum, const unsigned int column)
{
    const unsigned int wordStart = wordIdx;

    while (wordIdx < wordStr.length() && !isSingleCharToken(wordStr[wordIdx]))
        wordIdx++;

    const unsigned int wordEnd = wordIdx;
    if (wordIdx < wordStr.length() && isSingleCharToken(wordStr[wordIdx]) && wordIdx > 0)
        wordIdx--;

    const std::string tokenStr = wordStr.substr(wordStart, wordEnd);

    // Check if the token is a reserved keyword
    TokenType type = strToEnum<TokenType>(tokenStr);

    _tokens.push_back(Token(type, tokenStr, lineNum, column));
}

/**
 * @brief Goes through a whitespace separated word and extracts tokens from it
 *
 * @param wordStr The string containing the whitespace separated word
 * @param wordPos Where the word is in the line
 * @param lineNum The line number we are in
 */
void Tokenizer::tokenizeWord(const std::string &wordStr, const unsigned int wordPos,
                             const unsigned int lineNum)
{
    unsigned int wordIdx = 0;
    while (wordIdx < wordStr.length())
    {
        const char c = wordStr[wordIdx];
        if (c == '#')
            return;
        unsigned int column = wordPos + wordIdx + 1;
        if (isSingleCharToken(c))
        {
            const std::string tokenStr(1, c);
            const TokenType type = strToEnum<TokenType>(tokenStr);
            const Token token(type, tokenStr, lineNum, column);
            _tokens.push_back(token);
        }
        else
            addWord(wordIdx, wordStr, lineNum, column);
        wordIdx++;
    }
}

/**
 * @brief Destroy the Config Tokenizer object
 */
Tokenizer::~Tokenizer()
{
}
