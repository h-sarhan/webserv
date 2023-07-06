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

void Tokenizer::tokenizeLine(const std::string &line, const uint32_t num)
{
    (void) num;
    std::stringstream lineStream(line);
    tokenIterator lineStart(lineStream);
    for (tokenIterator it = lineStart; it != tokenIterator(); it++)
    {
        std::cout << "|" << *it << "|"
                  << " ";
    }
    std::cout << std::endl;
}

const std::vector<Token> &Tokenizer::tokens(void) const
{
    return _tkns;
}
