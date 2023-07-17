/**
 * @file ParseError.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the ParseError exception class
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "config/ParseError.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>

ParseError::ParseError(const std::string &errorMsg, const Token &token, const std::string &filename)

{
    std::stringstream msg;
    msg << BOLD << filename << ":" << token.line() << ":" << token.column() << RED
        << " error: " << RESET BOLD << errorMsg << RESET "\n"
        << "\n" RESET << getLine(filename, token.line()) << "\n"
        << std::string(token.column() - 1, ' ') << GREEN
        << std::string(token.contents().length(), '^') << std::endl;
    _errorMsg = msg.str();
}

ParseError::ParseError(const std::string &errorMsg, const std::string &filename)

{
    std::stringstream msg;
    msg << BOLD << filename << ":" << RED << " error: " << RESET BOLD << errorMsg << RESET
        << std::endl;
    _errorMsg = msg.str();
}

const char *ParseError::what() const throw()
{
    return _errorMsg.c_str();
}

ParseError::~ParseError() throw()
{
}
