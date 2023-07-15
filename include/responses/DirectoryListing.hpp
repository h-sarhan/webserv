/**
 * @file DirectoryListing.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines a function that generates an HTML directory listing for us
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef DIRECTORY_LISTING_HPP
#define DIRECTORY_LISTING_HPP

#include <string>

/**
 * @brief Generate a directory listing as an HTML page
 *
 * @param dirPath A path to a directory
 * @return const std::string The generated HTML page as a string
 */
const std::string generateDirectoryListing(const std::string &dirPath);

#endif
