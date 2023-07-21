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

#include <map>
#include <sstream>
#include <string>

// Colors
#define RED        "\x1b[31m"
#define RESET      "\x1b[0m"
#define YELLOW     "\x1b[33m"
#define GREEN      "\x1B[32m"
#define BOLD       "\x1b[1m"
#define WHITESPACE " \t\n\r\f\v"

// Get the size of an array if it is known at compile time
#define sizeOfArray(array) (sizeof(array) / sizeof(array[0]))

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
 * @brief Templated function to read a value from an stringstream
 *
 * @tparam T Type of the value to retrieve from the stringstream
 * @param stream The stringstream to retrieve the value from
 * @return T Retrieved value
 */
template <typename T> T getNext(std::iostream &stream)
{
    T val;
    stream >> val;
    return val;
}

// template <typename T> T getNext(char *bytes, size_t n)
// {
//     std::stringstream stream(std::string(bytes, bytes + n));
//     T val;
//     stream >> val;
//     return val;
// }

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
 * @brief Convert a hexadecimal string to an ASCII character
 *
 * @param str Hexadecimal string
 * @return char ASCII character
 */
unsigned int getHex(const std::string &str);

/**
 * @brief Decodes URL encoded characters from a URL and strips out any query parameters
 *
 * @param url URL to sanitize
 */
void sanitizeURL(std::string &url);

/**
 * @brief Get a line as a string from a file given the line number
 *
 * @param filename Name of the file
 * @param lineNum Line number
 * @return const std::string The line as a string
 */
const std::string getLine(const std::string &filename, const unsigned int lineNum);

/**
 * @brief Removes multiple ocurrences of a given character in a row
 * For example:
 * ppppppw -> pw
 * ./assets///web -> ./assets/web
 *
 * @param str String to modify
 * @param c Duplicate character
 */
void removeDuplicateChar(std::string &str, const char c);

/**
 * @brief Parses a key value file into a map. The file is expected to be in the following format
 *
 * KEY delim VALUE\n
 * KEY delim VALUE\n
 * ...
 *
 * @param filename Name of the file
 * @param delim Delimeter used
 * @return std::map<std::string, std::string> The file contents parsed into a map
 */
std::map<std::string, std::string> parseKeyValueFile(const std::string &filename, const char delim);

/**
 * @brief Unchunks a body recieved by a chunked HTTP request
 *
 * @param body Entire message body as a byte buffer
 * @param bodyLength Body length in bytes. This will be updated with the length of the unchunked
 * body
 * @return char* The new unchunked body, or NULL if the body is invalid
 */
char *unchunker(const char *body, size_t &bodyLength);

#endif
