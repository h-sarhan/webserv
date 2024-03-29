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

#define DEFAULT_ERROR(code)                                                                        \
    COMMON_HEAD "<title>" + toStr(code) +                                                          \
        "</title>\n"                                                                               \
        "\t</head>\n"                                                                              \
        "\t<body>\n"                                                                               \
        "\t\t<h1>ERROR " +                                                                         \
        toStr(code) +                                                                              \
        " </h1>\n"                                                                                 \
        "\t\t<hr>\n"                                                                               \
        "<img src=\"https://http.cat/" +                                                           \
        toStr(code) +                                                                              \
        ".jpg\" width=\"55%\" style=\"margin: auto; display: block\">"                             \
        "\n\t</body>\n</html>\n"

#include "requests/Resource.hpp"
#include <string>

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
 * @param responseCode Response code as an int
 * @param resource Resource requested by client
 * @return const std::string The generated HTML page as a string
 */
const std::string errorPage(unsigned int responseCode, const Resource &resource = Resource());

#endif
