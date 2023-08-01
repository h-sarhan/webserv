/**
 * @file env_utils.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of CGI env util functions
 * @date 2023-07-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cgiUtils.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>

void addToEnv(std::vector<char *> &env, std::string var)
{
    env.push_back(strdup(var.c_str()));
}

std::string getCGIVirtualPath(const Resource &res)
{
    std::string cgiFileName = baseName(res.path);
    size_t pos = res.originalRequest.find(cgiFileName);
    if (pos == std::string::npos)
        return res.originalRequest;   // this will never happen but still
    return res.originalRequest.substr(0, pos + cgiFileName.length());
}

void addHeadersToEnv(std::vector<char *> &env, headMap map)
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
SCRIPT_NAME: The virtual path of the script being executed. - everything in original request till
before PATH_INFO SCRIPT_FILENAME and REQUEST_FILENAME: The real path of the script being executed. -
everything in path PATH_INFO: The extra path information, as given in the requested URL. -
everything after SCRIPT_NAME in original request PATH_TRANSLATED: The virtual-to-real mapped version
of PATH_INFO URL: The full URI of the current request. It is made of the concatenation of
SCRIPT_NAME and PATH_INFO (if available.) - in our case original request
*/
void addPathEnv(std::vector<char *> &env, const Resource &res)
{
    std::string scriptName = getCGIVirtualPath(res);
    addToEnv(env, "SCRIPT_NAME=" + scriptName);
    addToEnv(env, "SCRIPT_FILENAME=" + baseName(res.path));
    // std::string pathInfo = res.originalRequest.substr(scriptName.length());
    std::string pathInfo = res.originalRequest;
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
    addToEnv(env, "REQUEST_URI=" + pathInfo);
    addToEnv(env, "URL=" + scriptName + pathInfo);
}

pid_t waitCGI(pid_t pid, int &status, int &sendErrCode)
{
    time_t startTime;
    time_t curTime;
    pid_t waitStatus;

    waitStatus = waitpid(pid, &status, WNOHANG);
    time(&startTime);
    while (waitStatus == 0)
    {
        waitStatus = waitpid(pid, &status, WNOHANG);
        time(&curTime);
        // gateway timed out, we waited for the cgi for too long;
        if (curTime - startTime > GATEWAY_TIMEOUT)
        {
            Log(ERR) << "CGI Error (504): WAITPID timed out" << std::endl;
            sendErrCode = 504;
            kill(pid, SIGTERM);   // terminating the cgi process explicitly to prevent it from
                                  // becoming a zombie
            break;
        }
    }
    return waitStatus;
}

int checkCGIError(pid_t pid, int sendErrCode, int waitStatus, int status)
{
    if (sendErrCode != 0)
    {
        Log(ERR) << "CGI error " << sendErrCode << std::endl;
        return sendErrCode;
    }
    if (waitStatus == -1)
    {
        Log(ERR) << "CGI error 502: waitpid returned error" << std::endl;
        return 502;   // bad gateway
    }
    if (waitStatus == pid && WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
    {
        Log(ERR) << "CGI error 502: process " << pid << " exited with error" << std::endl;
        return 502;
    }
    return EXIT_SUCCESS;
}

std::vector<char *> createExecArgs(std::string path)
{
    std::vector<char *> args;
    args.push_back(strdup(path.c_str()));
    args.push_back(NULL);
    return args;
}

pid_t startCGIProcess(std::vector<char *> env, int p[2], int &outFd)
{
    if (pipe(p) == -1)
        return -1;
    outFd = open(CGI_OUTFILE, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (outFd == -1)
    {
        Log(ERR) << "Could not open " << CGI_OUTFILE << " to write" << std::endl;
        close(p[0]);
        close(p[1]);
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        close(p[0]);
        close(p[1]);
        close(outFd);
        std::remove(CGI_OUTFILE);
        std::for_each(env.begin(), env.end(), free);
        return -1;
    }
    return pid;
}
