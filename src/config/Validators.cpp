/**
 * @file Validators.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines various input validators that will be used to check if user input is
 * valid
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/Validators.hpp"
#include <algorithm>
#include <cassert>
#include <limits>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <vector>

/**
 * @brief Checks if the character in a label is valid
 *
 * @param c
 * @return true
 * @return false
 */
static bool isValidLabelChar(const char &c)
{
    return c == '-' || std::isalnum(c);
}

/**
 * @brief Checks if the hostname label is valid
 *
 * @param label Host name label
 * @return true if the host name label is valid
 */
static bool validateLabel(const std::string &label)
{
    // Check length of label
    if (label.length() < 1 || label.length() > 63)
        return false;

    // Check for invalid hyphen placement
    if (*label.begin() == '-' || *(label.end() - 1) == '-')
        return false;

    // Get the number of valid characters
    const size_t numValidChars = std::count_if(label.begin(), label.end(), isValidLabelChar);

    return numValidChars == label.size();
}

/**
 * @brief Checks if the hostname is valid
 *
 * @param hostname The hostname
 * @return true if the host name is valid
 */
bool validateHostName(const std::string &hostname)
{
    // Check the hostname length
    if (hostname.length() == 0 || hostname.length() > 253)
        return false;

    // Generate labels by splitting on '.'
    std::vector<std::string> labels;

    std::string label;
    std::stringstream hostStream(hostname);
    while (std::getline(hostStream, label, '.'))
        labels.push_back(label);

    // Check if the label count is correct.
    // This will invalidate labels like "weeb..server" and alan.poe.
    const size_t numLabels = std::count(hostname.begin(), hostname.end(), '.') + 1;
    if (labels.size() != numLabels)
        return false;

    // Validate hostname labels
    const size_t numValidLabels = std::count_if(labels.begin(), labels.end(), validateLabel);

    return numValidLabels == numLabels;
}

/**
 * @brief Checks if an error response code is valid
 *
 * @param respCode The response code as a string
 * @return true if the error response is valid
 */
bool validateErrorResponse(const std::string &respCode)
{
    // Check the length
    if (respCode.length() != 3)
        return false;

    // Check the number of digits
    const size_t numDigits = std::count_if(respCode.begin(), respCode.end(), ::isdigit);

    if (numDigits != 3)
        return false;

    // Checks if the response is 4XX or 5XX
    return (*respCode.begin() == '4' || *respCode.begin() == '5');
}

/**
 * @brief Checks if the string given is a valid path to an HTML file
 *
 * @param htmlFile Path to HTML file
 * @return true if the path is valid
 */
bool validateHTMLFile(const std::string &htmlFile)
{
    struct stat info;

    if (htmlFile.empty())
        return false;

    // Check if the filename has a dot
    size_t dotPos = htmlFile.find_last_of('.');
    if (dotPos == std::string::npos)
        return false;

    // Get the extension
    std::string ext = htmlFile.substr(dotPos, htmlFile.length());

    // Transform extension to lowercase
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    // Check if the filename has an html extension
    if (ext != ".html")
        return false;

    // Check if the path to file/dir failed to open
    if (stat(htmlFile.c_str(), &info) != 0)
        return false;

    // Check if the path was to a file rather than a directory
    return info.st_mode & S_IFREG;
}

/**
 * @brief Checks if the string given is a valid directory
 *
 * @param dirPath Path to directory
 * @return true if the path is valid
 */
bool validateDirectory(const std::string &dirPath)
{
    struct stat info;

    // Check if the path to file/dir failed to open
    if (dirPath.empty() || stat(dirPath.c_str(), &info) != 0)
        return false;

    // Check if the path was to a directory
    return info.st_mode & S_IFDIR;
}

/**
 * @brief Checks if the body size is valid
 *
 * @param bodySizeStr Body size as a string
 * @return true if the body size is valid
 */
bool validateBodySize(const std::string &bodySizeStr)
{
    std::stringstream bodySizeStream(bodySizeStr);
    size_t bodySize = 0;

    bodySizeStream >> bodySize;

    // Checks if the conversion failed
    if (bodySizeStr.empty() || !bodySizeStream || !bodySizeStream.eof())
        return false;

    // Very arbitrary
    return bodySize >= 10 && bodySize <= std::numeric_limits<unsigned int>::max();
}

/**
 * @brief Checks if a port is valid
 *
 * @param portStr Port to validate
 * @return true if port is valid
 */
bool validatePort(const std::string &portStr)
{
    size_t port = 0;
    std::stringstream portStream(portStr);

    portStream >> port;

    // Checks if the the conversion failed or if there are additional characters
    if (portStr.empty() || !portStream || !portStream.eof())
        return false;

    return port >= 1 && port <= 65535;
}

// ! ADD MORE CHECKS TO URL VALIDATION
/**
 * @brief Check if a URL is valid
 * @todo Add more checks to the URL
 *
 * @param urlStr URL to validate
 * @return true if the url is valid
 */
bool validateURL(const std::string &urlStr)
{
    return !urlStr.empty();
}
