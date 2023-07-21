/**
 * @file Resource.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This defines the resource specified in a request
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "config/ServerBlock.hpp"
#include "enums/ResourceTypes.hpp"
#include <string>

/**
 * @brief This struct represents a resource requested by a client
 */
struct Resource
{
    // Create a resource object with a given type original requested URL, path, and route
    Resource(const ResourceType &type, const std::string &originalRequest = "",
             const std::string &path = "", const Route &route = Route());

    // Type of the resource
    ResourceType type;

    // Original request path
    std::string originalRequest;

    // Path to resource
    std::string path;

    // The route the resource came from
    Route route;
};
#endif
