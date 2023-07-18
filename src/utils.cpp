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
#include <fstream>

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
            // Get the url encoded character and check if it is valid
            const std::string hexStr(it + 1, it + 3);
            if (!std::isxdigit(hexStr[0]) || !std::isxdigit(hexStr[1]))
                continue;

            // Replace the `%` with the decoded hex character
            *it = getHex(hexStr);

            // Erase the next two characters and get a valid iterator
            it = url.erase(it + 1, it + 3);
        }
    }
}

const std::string getLine(const std::string &filename, const unsigned int lineNum)
{
    std::ifstream file(filename.c_str());

    if (!file)
        return "";
    unsigned int currLine = 0;
    std::string line;
    while (currLine != lineNum && std::getline(file, line))
        currLine++;
    return line;
}

void removeDuplicateChar(std::string &str, const char c)
{
    size_t dupPos = str.find(std::string(2, c));
    while (dupPos != std::string::npos)
    {
        str.erase(dupPos, 1);
        dupPos = str.find(std::string(2, c));
    }
}
