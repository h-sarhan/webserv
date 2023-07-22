/**
 * @file DefaultPages.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements functions that generate reusable HTML pages for us to respond with
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "responses/DefaultPages.hpp"
#include "utils.hpp"
#include <dirent.h>
#include <fstream>
#include <sstream>

// ! BUG: This does not always provide the correct URLs
const std::string directoryListing(const std::string &dirPath)
{
    throw std::runtime_error("This function is broken pls use the directoryListing function that "
                             "takes a Resource object");

    std::string html = COMMON_HEAD "\t\t<title>Directory listing for " + dirPath +
                       " </title>\n"
                       "\t</head>\n"
                       "\t<body>\n"
                       "\t\t<h1>Directory listing for " +
                       dirPath +
                       " </h1>\n"
                       "\t\t<hr>\n"
                       "\t\t<ul>\n";
    DIR *dirPtr = opendir(dirPath.c_str());
    if (dirPtr == NULL)
        return html +
               "\t\t<li>COULD NOT OPEN DIRECTORY</li>\n\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
    struct dirent *dir = readdir(dirPtr);
    while (dir)
    {
        std::string filename = dir->d_name;
        filename = (dir->d_type & DT_DIR) ? filename + "/" : filename;
        html += "\t\t\t<li><a href=\"" + filename + "\">" + filename + "</a></li>\n";
        dir = readdir(dirPtr);
    }
    closedir(dirPtr);
    return html + "\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
}

const std::string directoryListing(const Resource &dir)
{
    // ! NOT DONE YET
    std::string html = COMMON_HEAD "\t\t<title>Directory listing for " + dir.originalRequest +
                       " </title>\n"
                       "\t</head>\n"
                       "\t<body>\n"
                       "\t\t<h1>Directory listing for " +
                       dir.originalRequest +
                       " </h1>\n"
                       "\t\t<hr>\n"
                       "\t\t<ul>\n";
    DIR *dirPtr = opendir(dir.path.c_str());
    if (dirPtr == NULL)
        return html +
               "\t\t<li>COULD NOT OPEN DIRECTORY</li>\n\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
    struct dirent *dirElement = readdir(dirPtr);
    while (dirElement)
    {
        const std::string &filename = dirElement->d_name;
        // filename = (dirElement->d_type & DT_DIR) ? filename + "/" : filename;
        html += "\t\t\t<li><a href=\"" + filename + "\">" + filename + "</a></li>\n";
        dirElement = readdir(dirPtr);
    }
    closedir(dirPtr);
    return html + "\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
}

const std::string errorPage(unsigned int responseCode)
{
    const std::string &html = COMMON_HEAD "<title>" + toStr(responseCode) +
                              "</title>\n"
                              "\t</head>\n"
                              "\t<body>\n"
                              "\t\t<h1>ERROR " +
                              toStr(responseCode) +
                              " </h1>\n"
                              "\t\t<hr>\n"
                              "<img src=\"https://http.cat/" +
                              toStr(responseCode) +
                              ".jpg\" width=\"55%\" style=\"margin: auto; display: block\">"
                              "\n\t</body>\n</html>\n";
    return html;
}
