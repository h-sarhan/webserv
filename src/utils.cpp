/**
 * @file utils.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Implementing some helpful reusable functions
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils.hpp"

void rightTrimStr(std::string &str, const std::string &anyOf)
{
    str.erase(str.find_last_not_of(anyOf) + 1);
}

void leftTrimStr(std::string &str, const std::string &anyOf)
{
    str.erase(0, str.find_first_not_of(anyOf));
}

void trimStr(std::string &str, const std::string &anyOf)
{
    leftTrimStr(str, anyOf);
    rightTrimStr(str, anyOf);
}
