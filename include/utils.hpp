/**
 * @file utils.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Reusable functions that I dont know where to put
 * @date 2023-07-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

/**
 * @brief Templated function to read values from a string
 *
 * @tparam T Type of the value to retrieve from the string
 * @param str The string to retrieve the value from
 * @return T Retrieved value
 */
template <typename T> T fromStr(const std::string &str)
{
    T val;
    std::stringstream ss(str);
    ss >> val;
    return val;
}

/**
 * @brief Templated function to write to a string
 *
 * @tparam T Type of the value to be converted
 * @param val The value
 * @return std::string Generated string
 */
template <typename T> std::string toStr(const T &val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

/**
 * @brief
 *
 * @param str
 * @param anyOf
 */
void rightTrimStr(std::string &str, const std::string &anyOf);

/**
 * @brief
 *
 * @param str
 * @param anyOf
 */
void leftTrimStr(std::string &str, const std::string &anyOf);

/**
 * @brief
 *
 * @param str
 * @param anyOf
 */
void trimStr(std::string &str, const std::string &anyOf);

#endif
