/**
 * @file ResourceTypes.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the possible resource types that a client could request
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RESOURCE_TYPES_HPP
#define RESOURCE_TYPES_HPP

#include <string>

/**
 * @brief The types of resources that we can respond with
 */
typedef enum
{
    EXISTING_FILE,
    REDIRECTION,
    FORBIDDEN_METHOD,
    DIRECTORY,
    NOT_FOUND
} ResourceType;

#endif
