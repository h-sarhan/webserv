/**
 * @file DefaultPages.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file functions that generate reusable HTML pages for us
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef DEFAULT_PAGES_HPP
#define DEFAULT_PAGES_HPP

#define COMMON_HEAD                                                                                \
    "<html>\n"                                                                                     \
    "\t<head>\n"                                                                                   \
    "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"                \
    "\t\t<style>"                                                                                  \
    "\t\t\t* {"                                                                                    \
    "\t\t\t\tfont-family: \"Lucida Console\", \"Courier New\", monospace;\n"                       \
    "\t\t\t\tbackground-color: white;\n"                                                           \
    "\t\t\t\tfont-size: 22px;\n"                                                                   \
    "\t\t\t}"                                                                                      \
    "\t\t</style>"

#include "requests/Resource.hpp"
#include <string>

/**
 * @brief Generate a directory listing as an HTML page
 * ! This is broken pls use the directoryListing function that takes a Resource object
 * @param dirPath A path to a directory
 * @return const std::string The generated HTML page as a string
 */
const std::string directoryListing(const std::string &dirPath);

/**
 * @brief Generate a directory listing as an HTML page
 *
 * @param dir The directory requested by the user as a Resource object
 * @return const std::string The generated HTML page as a string
 */
const std::string directoryListing(const Resource &dir);

/**
 * @brief Generate an error page based on an HTTP status code
 *
 * @param responseCode
 * @return const std::string The generated HTML page as a string
 */
const std::string errorPage(unsigned int responseCode);

#endif
