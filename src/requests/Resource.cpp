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

Resource::Resource(const ResourceType &type, const std::string &resource)
    : type(type), path(resource)
{
}
