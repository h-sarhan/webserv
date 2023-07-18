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

// ! Give this class access to the route the request belongs to
struct Resource
{
    Resource(const ResourceType &type, const std::string &path = "");
    ResourceType type;
    std::string path;
};
#endif
