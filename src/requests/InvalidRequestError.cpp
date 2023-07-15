/**
 * @file InvalidRequestError.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the InvalidRequestError class
 * @date 2023-07-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "requests/InvalidRequestError.hpp"

InvalidRequestError::InvalidRequestError(const std::string &errorMsg) : _errorMsg(errorMsg)
{
}

const char *InvalidRequestError::what() const throw()
{
    return _errorMsg.c_str();
}

InvalidRequestError::~InvalidRequestError() throw()
{
}
