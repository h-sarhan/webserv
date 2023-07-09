/**
 * @file ConfigParseError.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the ConfigParseError exception class
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "ConfigParseError.hpp"
#include <fstream>

// ! DUMB IMPLEMENTATION: Consider storing the line somewhere
static const std::string getLine(const std::string &filename,
                                 const unsigned int errLine)
{
    std::ifstream file(filename.c_str());

    unsigned int lineNum = 0;
    std::string line;
    while (lineNum != errLine && std::getline(file, line))
        lineNum++;
    return line;
}

ConfigParseError::ConfigParseError(const std::string &errorMsg,
                                   const Token &token,
                                   const std::string &filename)
    : _token(token)
{
    std::stringstream msg;
    msg << BOLD << filename << ":" << _token.line() << ":" << _token.column()
        << RED << " error: " << RESET << BOLD << errorMsg << RESET "\n"
        << "\n" RESET << getLine(filename, _token.line()) << "\n"
        << std::string(_token.column() - 1, ' ') << GREEN
        << std::string(_token.contents().length(), '^') << std::endl;
    _errorMsg = msg.str();
}

const char *ConfigParseError::what() const throw()
{
    return _errorMsg.c_str();
}

ConfigParseError::~ConfigParseError() throw()
{
}
