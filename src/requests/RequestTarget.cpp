/**
 * @file RequestTarget.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the RequestTarget
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "requests/RequestTarget.hpp"

// ! Give it access to the route the request belongs to
RequestTarget::RequestTarget(const RequestType &type, const std::string &resource,
                             const std::string &route)
    : type(type), resource(resource), route(route)
{
}
