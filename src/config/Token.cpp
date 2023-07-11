/**
 * @file Token.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the Token class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/Token.hpp"
#include "config/Tokenizer.hpp"
#include <iostream>

Token::Token(const TokenType &type, const std::string &contents, unsigned int line,
             unsigned int column)
    : _type(type), _str(contents), _line(line), _column(column)
{
}

Token::Token(const Token &old)
    : _type(old._type), _str(old._str), _line(old._line), _column(old._column)
{
}

Token &Token::operator=(const Token &old)
{
    if (this == &old)
        return *this;
    this->_column = old._column;
    this->_line = old._line;
    this->_str = old._str;
    this->_type = old._type;
    return *this;
}

Token::~Token()
{
}

std::string Token::contents() const
{
    return this->_str;
}

TokenType Token::type() const
{
    return this->_type;
}

unsigned int Token::line() const
{
    return this->_line;
}

unsigned int Token::column() const
{
    return this->_column;
}

std::ostream &operator<<(std::ostream &os, const Token &tkn)
{
    std::cout << "{type: " << Tokenizer::tokenToStr[tkn.type()] << ", str: " << tkn.contents()
              << ", line: " << tkn.line() << ", col: " << tkn.column() << "}";
    return os;
}
