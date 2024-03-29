/**
 * @file HeaderData.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of header data util functions
 * @date 2023-07-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "responses/HeaderData.hpp"
#include "logger/Logger.hpp"
#include "utils.hpp"
#include <map>

std::string getStatus(int statusCode)
{
    switch (statusCode)
    {
    case 200:
        return "200 OK";
    case 201:
        return "201 Created";
    case 204:
        return "204 No Content";
    case 302:
        return "302 Found";
    case 307:
        return "307 Temporary Redirect";
    case 400:
        return "400 Bad Request";
    case 404:
        return "404 Not Found";
    case 405:
        return "405 Method Not Allowed";
    case 409:
        return "409 Conflict";
    case 413:
        return "413 Content Too Large";
    case 502:
        return "502 Bad Gateway";
    case 504:
        return "504 Gateway Timeout";
    case 503:
        return "503 Service Unavailable";
    }
    return "500 Internal Server Error";
}

std::string getContentType(std::string filename)
{
    static const std::map<std::string, std::string> mimeTypes =
        parseKeyValueFile("mime_types.txt", ' ');
    if (mimeTypes.empty())
    {
        logger::Log(ERR) << "Could not open mime_types.txt" << std::endl;
        return "application/octet-stream";
    }
    size_t extensionStart = filename.rfind(".");
    if (extensionStart != std::string::npos)
        if (mimeTypes.count(&filename[extensionStart]))
            return mimeTypes.at(&filename[extensionStart]);
    return "application/octet-stream";
}
