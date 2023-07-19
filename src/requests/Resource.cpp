/**
 * @file Resource.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the Resource class
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "requests/Resource.hpp"

/**
 * @brief Construct a new Resource object
 *
 * @param type Type of the resource
 * @param path Path to the resource
 */
Resource::Resource(const ResourceType &type, const std::string &path) : type(type), path(path)
{
}
