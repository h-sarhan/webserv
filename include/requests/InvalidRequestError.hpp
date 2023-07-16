/**
 * @file InvalidRequestError.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief An exception that is thrown if a request is invalid
 * @date 2023-07-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef INVALID_REQUEST_ERROR_HPP
#define INVALID_REQUEST_ERROR_HPP

#include <exception>
#include <string>

class InvalidRequestError : public std::exception
{
  private:
    std::string _errorMsg;

  public:
    InvalidRequestError(const std::string &errorMsg);
    virtual const char *what() const throw();
    ~InvalidRequestError() throw();
};

#endif
