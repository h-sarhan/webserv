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
    std::cout << token << std::endl;
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

void Tokenizer::tokenizeLine(const std::string &line, const uint32_t lineNum)
{
    std::stringstream lineStream(line);
    tokenIterator lineStart(lineStream);
    // TODO use std::for_each here
    for (tokenIterator it = lineStart; it != tokenIterator(); it++)
    {
        const std::string contents = *it;
        uint32_t col;
        if (lineStream.tellg() == -1)
            col = line.length() - contents.length();
        else
            col = (uint32_t) lineStream.tellg() - contents.length();
        Token token(WORD, contents, lineNum, col + 1);
        _tkns.push_back(token);
    }
}

const std::vector<Token> &Tokenizer::tokens(void) const
{
    return _tkns;
}
