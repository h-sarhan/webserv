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
#include <cassert>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

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

unsigned int getHex(const std::string &str)
{
    std::stringstream ss;
    unsigned int ch;
    ss << std::hex << str;
    ss >> ch;
    return ch;
}

std::string sanitizeURL(const std::string &url)
{
    std::string sanitizedURL;
    std::string queryStr;
    const size_t queryPos = url.find("?");
    if (queryPos != std::string::npos)
    {
        if (queryPos > 0)
            sanitizedURL = url.substr(0, queryPos);
        queryStr = url.substr(queryPos);
    }
    else
        sanitizedURL = url;

    // Remove duplicate slashes
    removeDuplicateChar(sanitizedURL, '/');

    // Replace '+' with ' '
    std::replace(sanitizedURL.begin(), sanitizedURL.end(), '+', ' ');

    // Decode % hexadecimal characters
    for (std::string::iterator it = sanitizedURL.begin(); it < sanitizedURL.end() - 2; it++)
    {
        if (*it != '%')
            continue;
        // Get the url encoded character and check if it is valid
        const std::string hexStr(it + 1, it + 3);
        if (!std::isxdigit(hexStr[0]) || !std::isxdigit(hexStr[1]))
            continue;

        // Replace the `%` with the decoded hex character
        *it = getHex(hexStr);

        // Erase the next two characters and get a valid iterator
        it = sanitizedURL.erase(it + 1, it + 3) - 1;
    }
    return sanitizedURL + queryStr;
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

// ! Use std::adjacent_find here
void removeDuplicateChar(std::string &str, const char c)
{
    size_t dupPos = str.find(std::string(2, c));
    while (dupPos != std::string::npos)
    {
        str.erase(dupPos, 1);
        dupPos = str.find(std::string(2, c));
    }
}

std::map<std::string, std::string> parseKeyValueFile(const std::string &filename, const char delim)
{
    std::ifstream fileStream(filename.c_str());
    std::map<std::string, std::string> keyVal;
    if (!fileStream)
    {
        std::cerr << "Error opening '" << filename << "'." << std::endl;
        return keyVal;
    }

    std::string key, value;
    char sep;
    while (fileStream)
    {
        fileStream >> std::skipws >> key >> std::noskipws >> sep >> std::skipws >> value;
        if (sep != delim)
        {
            std::cerr << "Error reading '" << filename << "'. Invalid delimiter" << std::endl;
            return std::map<std::string, std::string>();
        }
        keyVal.insert(std::make_pair(key, value));
    }
    return keyVal;
}

bool exists(const std::string &path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0;
}

bool isFile(const std::string &path)
{
    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        return false;
    return info.st_mode & S_IFREG;
}

bool isDir(const std::string &path)
{
    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        return false;
    return info.st_mode & S_IFDIR;
}

std::string headerToEnv(const std::string &header)
{
    std::string cgiEnv("HTTP_" + header);

    std::transform(cgiEnv.begin(), cgiEnv.end(), cgiEnv.begin(), ::toupper);
    std::replace(cgiEnv.begin(), cgiEnv.end(), '-', '_');
    return cgiEnv;
}

std::string dirName(const std::string &path)
{
    if (path.empty())
        return ".";
    if (path == "/")
        return "/";
    const size_t lastSlash = path.find_last_of("/");
    if (lastSlash == std::string::npos)
        return ".";
    return path.substr(0, lastSlash);
}

std::string baseName(const std::string &path)
{
    if (path.empty())
        return ".";
    if (path == "/")
        return "/";
    const size_t lastSlash = path.find_last_of("/");
    if (lastSlash == std::string::npos)
        return path;
    return path.substr(lastSlash + 1);
}
