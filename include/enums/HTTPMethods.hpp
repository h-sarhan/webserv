/**
 * @file HTTPMethods.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file HTTP Methods as enums
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef HTTP_METHODS_HPP
#define HTTP_METHODS_HPP

/**
 * @brief The HTTP methods that our server handles
 */
typedef enum
{
    GET,
    POST,
    PUT,
    DELETE
} HTTPMethod;

#endif
