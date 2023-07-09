/**
 * @file ConfigParseError.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Custom exception class when an error is encountered during config file
 * parsing
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CONFIG_PARSE_ERROR_HPP
#define CONFIG_PARSE_ERROR_HPP

#define RED    "\x1b[31m"
#define RESET  "\x1b[0m"
#define YELLOW "\x1b[33m"
#define GREEN  "\x1B[32m"
#define BOLD   "\x1b[1m"

#include "Token.hpp"
#include <exception>
#include <iostream>
#include <string>

class ConfigParseError : public std::exception
{
  private:
    std::string _errorMsg;
    const Token &_token;

  public:
    ConfigParseError(const std::string &errorMsg, const Token &token,
                     const std::string &filename);
    virtual const char *what() const throw();
    ~ConfigParseError() throw();
};
#endif
