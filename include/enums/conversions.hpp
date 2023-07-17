/**
 * @file conversions.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Helpful functions to convert enums to strings and vice versa
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ENUM_CONVERSIONS_HPP
#define ENUM_CONVERSIONS_HPP

#define sizeOfArray(array) sizeof(array) / sizeof(array[0])

#include "enums/HTTPMethods.hpp"
#include "enums/RequestTypes.hpp"
#include "enums/TokenTypes.hpp"

std::string enumToStr(const HTTPMethod &enumVal);
std::string enumToStr(const RequestType &enumVal);
std::string enumToStr(const TokenType &enumVal);

template <typename E> E strToEnum(const std::string &str);

#endif
