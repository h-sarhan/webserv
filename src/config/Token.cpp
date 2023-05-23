/**
 * @file Token.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the Token class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Token.hpp"

Token::Token(const TokenType &type, const std::string &contents, uint32_t line,
             uint32_t column)
    : _type(type), _str(contents), _line(line), _column(column)
{
}

Token::Token(const Token &old)
    : _type(old._type), _str(old._str), _line(old._line), _column(old._column)
{
}

std::string Token::contents(void) const
{
    return this->_str;
}

TokenType Token::type(void) const
{
    return this->_type;
}

uint32_t Token::line(void) const
{
    return this->_line;
}

uint32_t Token::column(void) const
{
    return this->_column;
}
