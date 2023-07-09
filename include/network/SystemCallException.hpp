/**
 * @file SystemCallException.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Custom exception class for when a system call fails
 * 		  We can have general exceptions - "webserv: <error msg>"
 * 		  Or specific exceptions for a function - "<function name>: <error msg>"
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SYSTEM_CALL_EXCEPTION_HPP
#define SYSTEM_CALL_EXCEPTION_HPP

#include <exception>
#include <iostream>
#include <string>

class SystemCallException : public std::exception
{
  private:
    std::string errorMessage;

  public:
    SystemCallException(const std::string &functionName,
                        const std::string &errorMsg)
        : errorMessage(functionName + ": " + errorMsg)
    {
    }
    SystemCallException(const std::string &errorMsg)
        : errorMessage("webserv: " + errorMsg)
    {
    }
    virtual const char *what() const throw()
    {
        return errorMessage.c_str();
    }
    ~SystemCallException() throw()
    {
    }
};

#endif