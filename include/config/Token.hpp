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
 * @brief This class represents a single token. A token refers to a single
 * component or piece of a larger group of text.
 * A Token object stores the type of the token as well as its contents.
 * The location of where the token is found in the text is also stored.
 */
class Token
{
  private:
    TokenType _type;
    std::string _str;
    unsigned int _line;
    unsigned int _column;

  public:
    /**
     * @brief Construct a new Token object
     *
     * @param type Type of the token
     * @param contents The string that makes up the token
     * @param lineNum The line the token was found on
     * @param column The column number the token was found on
     */
    Token(const TokenType &type, const std::string &contents, unsigned int lineNum,
          unsigned int column);

    /**
     * @brief Token copy assignment operator
     *
     * @param old Old token
     * @return Token& Token reference
     */
    Token &operator=(const Token &old);

    /**
     * @brief Token copy constructor
     *
     * @param old The token we are copyng from
     */
    Token(const Token &old);

    /**
     * @brief Destroy the Token object
     *
     */
    ~Token();

    /**
     * @brief Get the contents of the token
     *
     * @return std::string Token contents
     */
    std::string contents() const;

    /**
     * @brief Get the token type
     *
     * @return TokenType token type
     */
    TokenType type() const;

    /**
     * @brief The line the token was found on
     *
     * @return unsigned int
     */
    unsigned int line() const;

    /**
     * @brief The column number of the token
     *
     * @return unsigned int Column number
     */
    unsigned int column() const;
};

std::ostream &operator<<(std::ostream &os, const Token &tkn);

#endif
