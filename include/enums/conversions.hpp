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

#include <string>

/**
 * @brief Convert an enum to a string
 *
 * @tparam E Enum type
 * @param enumVal The enum to convert
 * @return std::string The converted enum as a string
 */
template <typename E> std::string enumToStr(const E &enumVal);

/**
 * @brief Converts an string to an enum
 *
 * @tparam E Enum type
 * @param str String to convert
 * @return E The enum
 */
template <typename E> E strToEnum(const std::string &str);

#endif
