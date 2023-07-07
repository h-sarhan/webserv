/**
 * @file Token.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the Token class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef TOKEN
#define TOKEN

#include "TokenTypes.hpp"
#include "common.hpp"

/**
 * @brief This class represents a single token. A token refers to a single
 * component or piece of a larger group of text.
 * A Token object stores the type of the token as well as its contents.
 * The location of where the token is found in the text is also stored.
 */
class Token
{
  private:
    const TokenType _type;
    const std::string _str;
    const uint32_t _line;
    const uint32_t _column;

  public:
    /**
     * @brief Construct a new Token object
     *
     * @param type Type of the token
     * @param contents The string that makes up the token
     * @param line The line the token was found on
     * @param column The column number the token was found on
     */
    Token(const TokenType &type, const std::string &contents, uint32_t lineNum,
          uint32_t column);

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
    ~Token(void);

    /**
     * @brief Get the contents of the token
     *
     * @return std::string Token contents
     */
    std::string contents(void) const;

    /**
     * @brief Get the token type
     *
     * @return TokenType token type
     */
    TokenType type(void) const;

    /**
     * @brief The line the token was found on
     *
     * @return uint32_t
     */
    uint32_t line(void) const;

    /**
     * @brief The column number of the token
     *
     * @return uint32_t Column number
     */
    uint32_t column(void) const;

  private:
    Token &operator=(const Token &old);
};

std::ostream &operator<<(std::ostream &os, const Token &tkn);

#endif
