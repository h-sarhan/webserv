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
#include <fstream>
#include <sstream>

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

ParseError::ParseError(const std::string &errorMsg, const Token &token,
                       const std::string &filename)

{
    std::stringstream msg;
    msg << BOLD << filename << ":" << token.line() << ":" << token.column()
        << RED << " error: " << RESET BOLD << errorMsg << RESET "\n"
        << "\n" RESET << getLine(filename, token.line()) << "\n"
        << std::string(token.column() - 1, ' ') << GREEN
        << std::string(token.contents().length(), '^') << std::endl;
    _errorMsg = msg.str();
}

ParseError::ParseError(const std::string &errorMsg, const std::string &filename)

{
    std::stringstream msg;
    msg << BOLD << filename << ":" << RED << " error: " << RESET BOLD
        << errorMsg << RESET << std::endl;
    _errorMsg = msg.str();
}

const char *ParseError::what() const throw()
{
    return _errorMsg.c_str();
}

ParseError::~ParseError() throw()
{
}
