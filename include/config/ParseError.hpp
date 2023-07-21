/**
 * @file ParseError.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Custom exception class for when an error is encountered during config file parsing
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef PARSE_ERROR_HPP
#define PARSE_ERROR_HPP

#include "Token.hpp"

class ParseError : public std::exception
{
  private:
    std::string _errorMsg;

  public:
    // Constructs a parse error with a simple error message
    ParseError(const std::string &errorMsg, const std::string &filename);

    // Constructs a ParseError with an error message based on the token
    ParseError(const std::string &errorMsg, const Token &token, const std::string &filename);
    virtual const char *what() const throw();
    ~ParseError() throw();
};
#endif
