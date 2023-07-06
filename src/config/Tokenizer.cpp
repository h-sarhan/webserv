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

Tokenizer::Tokenizer(const std::string &filename)
    : _src(filename), _start(0), _current(0)
{
}

const std::vector<Token> &Tokenizer::tokens(void) const
{
    return _tkns;
}
