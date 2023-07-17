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
 * @brief Trim characters at the end of the string
 *
 * @param str String to trim
 * @param anyOf Trim any of these characters
 */
void rightTrimStr(std::string &str, const std::string &anyOf);

/**
 * @brief Trim characters at the start of the string
 *
 * @param str String to trim
 * @param anyOf Trim any of these characters
 */
void leftTrimStr(std::string &str, const std::string &anyOf);

/**
 * @brief Trim characters at the start and end of the string
 *
 * @param str String to trim
 * @param anyOf Trim any of these characters
 */
void trimStr(std::string &str, const std::string &anyOf);

/**
 * @brief Replace a character to `to` if it is `from`
 *
 * @tparam from Character to replace
 * @tparam to Which character to replace with
 * @param c The character to check
 */
template <char from, char to> char replaceChar(char &c)
{
    if (c == from)
        return to;
    return c;
}

/**
 * @brief Convert a hexadecimal string to an ASCII character
 *
 * @param str Hexadecimal string
 * @return char ASCII character
 */
char getHex(const std::string &str);

/**
 * @brief Decodes URL encoded characters from a URL and strips out any query parameters
 *
 * @param url URL to sanitize
 */
void sanitizeURL(std::string &url);

#endif
