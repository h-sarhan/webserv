/**
 * @file RequestTarget.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This defines the resource specified in a request
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef REQUEST_TARGET_HPP
#define REQUEST_TARGET_HPP

#include "enums/RequestTypes.hpp"

// ! Give it access to the route the request belongs to
struct RequestTarget
{
    RequestTarget(const RequestType &type, const std::string &resource = "",
                  const std::string &route = "");
    RequestType type;
    std::string resource;
    std::string route;
};
#endif
