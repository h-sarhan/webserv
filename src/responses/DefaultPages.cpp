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

const std::string directoryListing(const Resource &dir)
{
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
        return html + "\t\t<li>COULD NOT OPEN"
                      "DIRECTORY</ li>\n\t\t</ ul>\n\t\t<hr>\n\t</ body>\n</ html>\n ";
    struct dirent *dirElement = readdir(dirPtr);
    while (dirElement)
    {
        const std::string &filename = dirElement->d_name;
        const std::string &url = sanitizeURL(dir.originalRequest + "/" + filename);
        html += "\t\t\t<li><a href=\"" + url + "\">" + filename + "</a></li>\n";
        dirElement = readdir(dirPtr);
    }
    closedir(dirPtr);
    return html + "\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
}

const std::string errorPage(unsigned int responseCode, const Resource &resource)
{
    const std::map<unsigned int, std::string> &errorPages = resource.config.first.errorPages;
    if (errorPages.count(responseCode) == 0)
        return DEFAULT_ERROR(responseCode);

    std::ifstream errorFile(errorPages.at(responseCode).c_str());
    if (!errorFile)
        return DEFAULT_ERROR(responseCode);
    std::stringstream errorFileStream;
    errorFileStream << errorFile.rdbuf();
    return errorFileStream.str();
}
