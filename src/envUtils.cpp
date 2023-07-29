/**
 * @file env_utils.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of CGI env util functions
 * @date 2023-07-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "envUtils.hpp"
#include "utils.hpp"
#include <cstring>

void addToEnv(std::vector<char *>& env, std::string var)
{
    env.push_back(strdup(var.c_str()));
}

std::string getCGIVirtualPath(const Resource &res)
{
    std::string cgiFileName = baseName(res.path);
    size_t pos = res.originalRequest.find(cgiFileName);
    if (pos == std::string::npos)
        return res.originalRequest; // this will never happen but still
    return res.originalRequest.substr(0, pos + cgiFileName.length());
}

void addHeadersToEnv(std::vector<char *>& env, headMap map)
{
    if (map.count("content-type"))
        addToEnv(env, "CONTENT_TYPE=" + map.at("content-type"));
    if (map.count("content-length"))
        addToEnv(env, "CONTENT_LENGTH=" + map.at("content-length"));
    for (headMap::iterator it = map.begin(); it != map.end(); it++)
        if (it->first != "content-type" && it->first != "content-length")
            addToEnv(env, headerToEnv(it->first) + "=" + it->second);
    env.push_back(NULL);
}

/*
SCRIPT_NAME: The virtual path of the script being executed. - everything in original request till before PATH_INFO
SCRIPT_FILENAME and REQUEST_FILENAME: The real path of the script being executed. - everything in path 
PATH_INFO: The extra path information, as given in the requested URL. - everything after SCRIPT_NAME in original request
PATH_TRANSLATED: The virtual-to-real mapped version of PATH_INFO 
URL: The full URI of the current request. It is made of the concatenation of SCRIPT_NAME and PATH_INFO (if available.) - in our case original request
*/
void addPathEnv(std::vector<char *>& env, const Resource& res)
{
    std::string scriptName = getCGIVirtualPath(res);
    addToEnv(env, "SCRIPT_NAME=" + scriptName);
    addToEnv(env, "SCRIPT_FILENAME=" + baseName(res.path));
    std::string pathInfo = res.originalRequest.substr(scriptName.length());
    if (pathInfo.length() != 0)
    {
        size_t queryStart = pathInfo.find("?");
        if (queryStart != std::string::npos)
        {
            addToEnv(env, "QUERY_STRING=" + pathInfo.substr(queryStart + 1));
            pathInfo = pathInfo.substr(0, queryStart);
        }
        if (pathInfo.length() != 0)
        {
            addToEnv(env, "PATH_INFO=" + pathInfo);
            addToEnv(env, "PATH_TRANSLATED=." + pathInfo);
        }
    }
    addToEnv(env, "URL=" + scriptName + pathInfo);
}
