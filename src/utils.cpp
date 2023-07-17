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
#include <algorithm>

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

char getHex(const std::string &str)
{
    std::stringstream ss;
    unsigned int ch;
    ss << std::hex << str;
    ss >> ch;
    return ch;
}

void sanitizeURL(std::string &url)
{
    // Trim query parameters
    const size_t queryPos = url.find("?");
    if (queryPos != std::string::npos)
        url.erase(queryPos);

    // Replace '+' with ' '
    std::transform(url.begin(), url.end(), url.begin(), replaceChar<'+', ' '>);

    // Decode % hexadecimal characters
    for (std::string::iterator it = url.begin(); it != url.end(); it++)
    {
        if (*it == '%' && (url.end() - it) > 2)
        {
            const std::string hexStr(it + 1, it + 3);

            if (!std::isxdigit(hexStr[1]) || std::isxdigit(hexStr[2]))
                continue;

            // Replace the `%` with the hex character
            *it = getHex(hexStr);

            // Erase the next two characters
            url.erase(it + 1, it + 3);

            // Get a valid iterator to the start of the URL
            it = url.begin();
        }
    }
}

// void sanitizeURLTests()
// {
//     std::string url = "%";
//     sanitizeURL(url);
//     assert(url == "%");

//     url = "%20%+";
//     sanitizeURL(url);
//     assert(url == " % ");

//     url = "%20%+?kuybdluyb";
//     sanitizeURL(url);
//     assert(url == " % ");

//     url = "%20%+?kuybdluyb%";
//     sanitizeURL(url);
//     assert(url == " % ");

//     url = "%20%+%%%%?kuybdluyb%";
//     sanitizeURL(url);
//     assert(url == " % %%%%");

//     url = "%3d%3d%35%";
//     sanitizeURL(url);
//     assert(url == "==5%");

//     url = "?%3d%3d%35%";
//     sanitizeURL(url);
//     assert(url == "");
// }
