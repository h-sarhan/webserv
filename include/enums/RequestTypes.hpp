/**
 * @file RequestTypes.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the possible request types that we could get
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef REQUEST_TYPES_HPP
#define REQUEST_TYPES_HPP

#include <string>

/**
 * @brief The HTTP methods that our server handles
 */
typedef enum
{
    OK,
    REDIRECTION,
    METHOD_NOT_ALLOWED,
    DIRECTORY,
    NOT_FOUND
} RequestType;

RequestType strToRequestType(const std::string &str);
std::string requestTypeToStr(RequestType tkn);

#endif
