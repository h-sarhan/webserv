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
    "\t\t\t\tfont-family: \"Comic Sans MS\", \"Comic Sans\", cursive;\n"                           \
    "\t\t\t\tbackground-color: lightgreen;\n"                                                      \
    "\t\t\t\tfont-size: 24px;\n"                                                                   \
    "\t\t\t}"                                                                                      \
    "\t\t</style>"

#include <string>

/**
 * @brief Generate a directory listing as an HTML page
 *
 * @param dirPath A path to a directory
 * @return const std::string The generated HTML page as a string
 */
const std::string directoryListing(const std::string &dirPath);

/**
 * @brief Generate an error page based on an HTTP status code
 *
 * @param responseCode
 * @return const std::string The generated HTML page as a string
 */
const std::string errorPage(unsigned int responseCode);

#endif
