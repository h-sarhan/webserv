/**
 * @file DefaultPages.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the directory listing generation function
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "responses/DefaultPages.hpp"
#include <dirent.h>
#include <fstream>
#include <sstream>

const std::string directoryListing(const std::string &dirPath)
{

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
               "\t\t\t<li>COULD NOT OPEN DIR</li>\n\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
    struct dirent *dir = readdir(dirPtr);
    while (dir)
    {
        std::string filename = dir->d_name;
        filename = (dir->d_type & DT_DIR) ? filename + "/" : filename;
        html += "\t\t\t<li><a href=\"" + filename + "\">" + filename + "</a></li>\n";
        dir = readdir(dirPtr);
    }
    closedir(dirPtr);
    html += "\t\t</ul>\n\t\t<hr>\n\t</body>\n</html>\n";
    // std::ofstream file("dir_listing.html");
    // if (!file.is_open())
    //     exit(1);
    // file << html;
    return html;
}

const std::string errorPage(unsigned int responseCode)
{
    std::stringstream responseStream;
    responseStream << responseCode;
    std::string html = COMMON_HEAD "<title>" + responseStream.str() +
                       "</title>\n"
                       "\t</head>\n"
                       "\t<body>\n"
                       "\t\t<h1>ERROR " +
                       responseStream.str() +
                       " </h1>\n"
                       "\t\t<hr>\n"
                       "<img src=\"https://http.cat/" +
                       responseStream.str() +
                       ".jpg\" width=\"55%\" style=\"margin: auto; display: block\">"
                       "\n\t</body>\n</html>\n";
    return html;
}
