/**
 * @file Token.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the Token class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "enums/TokenTypes.hpp"
#include <string>

/**
 * @brief This class represents a single token in our config file
 * The location of where the token is found in the file is also stored.
 */
class Token
{
  private:
    TokenType _type;
    std::string _str;
    unsigned int _line;
    unsigned int _column;

  public:
    Token(const TokenType &type, const std::string &contents, unsigned int lineNum,
          unsigned int column);
    Token(const Token &old);
    Token &operator=(const Token &old);
    ~Token();

    // Getters for token attributes
    std::string contents() const;
    TokenType type() const;
    unsigned int line() const;
    unsigned int column() const;
};

// Print a token object
std::ostream &operator<<(std::ostream &os, const Token &tkn);

#endif
