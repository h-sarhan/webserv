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

/**
 * @brief Construct a new ParseError with an error message based on the Token where the error
 * occurred
 *
 * @param errorMsg Error message
 * @param token Token where the error ocurred
 * @param filename Name of the config file
 */
ParseError::ParseError(const std::string &errorMsg, const Token &token, const std::string &filename)

{
    _errorMsg = BOLD + filename + ":" + toStr(token.line()) + ":" + toStr(token.column()) +
                RED " error: " + RESET BOLD + errorMsg + RESET "\n\n" +
                getLine(filename, token.line()) + "\n" + std::string(token.column() - 1, ' ') +
                GREEN + std::string(token.contents().length(), '^') + RESET "\n";
}

/**
 * @brief Construct a new ParseError with a simple error message
 *
 * @param errorMsg Error message
 * @param filename Name of the config file
 */
ParseError::ParseError(const std::string &errorMsg, const std::string &filename)

{
    _errorMsg = BOLD + filename + ":" RED " error: " RESET BOLD + errorMsg + RESET "\n";
}

/**
 * @brief Get error message
 *
 * @return const char* Error message as a char *
 */
const char *ParseError::what() const throw()
{
    return _errorMsg.c_str();
}

/**
 * @brief Destroy the Parse Error object
 */
ParseError::~ParseError() throw()
{
}
