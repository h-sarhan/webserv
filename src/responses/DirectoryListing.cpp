/**
 * @file DirectoryListing.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the directory listing generation function
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "responses/DirectoryListing.hpp"
#include <dirent.h>
#include <fstream>

const std::string generateDirectoryListing(const std::string &dirPath)
{
    std::string html =
        "<html>\n"
        "\t<head>\n"
        "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
        "\t\t<title>Directory listing for " +
        dirPath +
        " </title>\n"
        "\t\t<style>"
        "\t\t\t* {"
        "\t\t\t\tfont-family: \"Comic Sans MS\", \"Comic Sans\", cursive;\n"
        "\t\t\t\tbackground-color: lightgreen;\n"
        "\t\t\t\tfont-size: 24px;\n"
        "\t\t\t}"
        "\t\t</style>"
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
