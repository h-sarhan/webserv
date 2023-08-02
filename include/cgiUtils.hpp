/**
 * @file env_utils.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief All util functions used by Connection to prepare the CGI environment
 * @date 2023-07-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CGI_UTILS_HPP
#define CGI_UTILS_HPP

#define GATEWAY_TIMEOUT 10
#define CGI_OUTFILE     "cgiOutFile"
#include "logger/Logger.hpp"
#include <requests/Resource.hpp>
#include <sys/wait.h>

typedef std::map<std::string, std::string> headMap;
using logger::Log;

void addToEnv(std::vector<char *> &env, std::string var);
std::string getCGIVirtualPath(const Resource &res);
void addHeadersToEnv(std::vector<char *> &env, headMap map);
void addPathEnv(std::vector<char *> &env, const Resource &res);
pid_t waitCGI(pid_t pid, int &status, int &sendErrCode);
int checkCGIError(pid_t pid, int sendErrCode, int waitStatus, int status);
std::vector<char *> createExecArgs(std::string path);
pid_t startCGIProcess(std::vector<char *> env, int p[2], int &outFd);

#endif
