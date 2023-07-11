/**
 * @file ParseError.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Custom exception class when an error is encountered during config file
 * parsing
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef PARSE_ERROR_HPP
#define PARSE_ERROR_HPP

#define RED    "\x1b[31m"
#define RESET  "\x1b[0m"
#define YELLOW "\x1b[33m"
#define GREEN  "\x1B[32m"
#define BOLD   "\x1b[1m"

#include "Token.hpp"

class ParseError : public std::exception
{
  private:
    std::string _errorMsg;

  public:
    ParseError(const std::string &errorMsg, const Token &token, const std::string &filename);
    ParseError(const std::string &errorMsg, const std::string &filename);
    virtual const char *what() const throw();
    ~ParseError() throw();
};
#endif
