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
#include "enums/conversions.hpp"
#include <iostream>

/**
 * @brief Construct a new Token object
 *
 * @param type Type of the token
 * @param contents The string that makes up the token
 * @param lineNum The line the token was found on
 * @param column The column number the token was found on
 */
Token::Token(const TokenType &type, const std::string &contents, unsigned int line,
             unsigned int column)
    : _type(type), _str(contents), _line(line), _column(column)
{
}

/**
 * @brief Token copy constructor
 *
 * @param old The token we are copyng from
 */
Token::Token(const Token &old)
    : _type(old._type), _str(old._str), _line(old._line), _column(old._column)
{
}

/**
 * @brief Token copy assignment operator
 *
 * @param old Old token
 * @return Token& Token reference
 */
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

/**
 * @brief Destroy the Token object
 *
 */
Token::~Token()
{
}

/**
 * @brief Get the contents of the token
 *
 * @return std::string Token contents
 */
std::string Token::contents() const
{
    return this->_str;
}

/**
 * @brief Get the token type
 *
 * @return TokenType token type
 */
TokenType Token::type() const
{
    return this->_type;
}

/**
 * @brief The line the token was found on
 *
 * @return unsigned int
 */
unsigned int Token::line() const
{
    return this->_line;
}

/**
 * @brief The column number of the token
 *
 * @return unsigned int Column number
 */
unsigned int Token::column() const
{
    return this->_column;
}

std::ostream &operator<<(std::ostream &os, const Token &tkn)
{
    os << "{type: " << enumToStr(tkn.type()) << ", str: " << tkn.contents()
       << ", line: " << tkn.line() << ", col: " << tkn.column() << "}";
    return os;
}
