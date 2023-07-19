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

#include "enums/ResourceTypes.hpp"
#include <string>

// ! Give this class access to the route the request belongs to
/**
 * @brief This struct represents a resource requested by a client
 */
struct Resource
{
    // Create a resource object with a given type and path
    Resource(const ResourceType &type, const std::string &path = "");

    // Type of the resource
    ResourceType type;

    // Path to resource
    std::string path;
};
#endif
