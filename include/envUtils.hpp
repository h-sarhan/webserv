/**
 * @file env_utils.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief All util functions used by Connection to prepare the CGI environment
 * @date 2023-07-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ENV_UTILS_HPP
#define ENV_UTILS_HPP

#include <requests/Resource.hpp>

typedef std::map<std::string, std::string> headMap;

void addToEnv(std::vector<char *> &env, std::string var);
std::string getCGIVirtualPath(const Resource &res);
void addHeadersToEnv(std::vector<char *> &env, headMap map);
void addPathEnv(std::vector<char *> &env, const Resource &res);

#endif
