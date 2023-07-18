/**
 * @file Validators.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file declares various input validators used throughout parsing
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef VALIDATORS_HPP
#define VALIDATORS_HPP

#include <string>

// Various input validators associated with parsing the config file
bool validateHostName(const std::string &hostname);
bool validateErrorResponse(const std::string &respCode);
bool validateHTMLFile(const std::string &htmlFile);
bool validateDirectory(const std::string &dirPath);
bool validatePort(const std::string &portStr);
bool validateURL(const std::string &urlStr);
bool validateBodySize(const std::string &bodySizeStr);
bool validateHTMLMethod(const std::string &bodySizeStr);

// A function that tests out all the input validators
void inputValidatorTests();

#endif
