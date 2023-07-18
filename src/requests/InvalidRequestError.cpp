/**
 * @file InvalidRequestError.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the InvalidRequestError exception class
 * @date 2023-07-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "requests/InvalidRequestError.hpp"

/**
 * @brief Construct a new InvalidRequestError exception with the provided error message
 *
 * @param errorMsg Error message
 */
InvalidRequestError::InvalidRequestError(const std::string &errorMsg) : _errorMsg(errorMsg)
{
}

/**
 * @brief Return The error message as a C-string
 *
 * @return const char*
 */
const char *InvalidRequestError::what() const throw()
{
    return _errorMsg.c_str();
}

InvalidRequestError::~InvalidRequestError() throw()
{
}
