/**
 * @file enumConversions.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements several enum to string and string to enum conversion functions
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "enums/HTTPMethods.hpp"
#include "enums/ResourceTypes.hpp"
#include "enums/TokenTypes.hpp"
#include "enums/conversions.hpp"
#include "utils.hpp"

/**
 * @brief HTTPMethod template specialization for enumToStr
 *
 * @see enumToStr
 * @param enumVal Enum value
 * @return std::string Enum value as a string
 */
template <> std::string enumToStr(const HTTPMethod &enumVal)
{
    switch (enumVal)
    {
    case GET:
        return "GET";
    case POST:
        return "POST";
    case PUT:
        return "PUT";
    case DELETE:
        return "DELETE";
    case OTHER:
        return "OTHER";
    case HEAD:
        return "HEAD";
    }
}

/**
 * @brief ResourceType template specialization for enumToStr
 *
 * @see enumToStr
 * @param enumVal Enum value
 * @return std::string Enum value as a string
 */
template <> std::string enumToStr(const ResourceType &enumVal)
{
    switch (enumVal)
    {
    case EXISTING_FILE:
        return "EXISTING_FILE";
    case REDIRECTION:
        return "REDIRECTION";
    case NOT_FOUND:
        return "NOT_FOUND";
    case FORBIDDEN_METHOD:
        return "FORBIDDEN_METHOD";
    case DIRECTORY:
        return "DIRECTORY";
    case INVALID_REQUEST:
        return "INVALID_REQUEST";
    case NO_MATCH:
        return "NO_MATCH";
    }
}

/**
 * @brief TokenType template specialization for enumToStr
 *
 * @see enumToStr
 * @param enumVal Enum value
 * @return std::string Enum value as a string
 */
template <> std::string enumToStr(const TokenType &enumVal)
{
    switch (enumVal)
    {
    case LEFT_BRACE:
        return "LEFT_BRACE";
    case RIGHT_BRACE:
        return "RIGHT_BRACE";
    case POUND:
        return "POUND";
    case SEMICOLON:
        return "SEMICOLON";
    case WORD:
        return "WORD";
    case SERVER:
        return "SERVER";
    case LISTEN:
        return "LISTEN";
    case SERVER_NAME:
        return "SERVER_NAME";
    case ERROR_PAGE:
        return "ERROR_PAGE";
    case LOCATION:
        return "LOCATION";
    case TRY_FILES:
        return "TRY_FILES";
    case BODY_SIZE:
        return "BODY_SIZE";
    case METHODS:
        return "METHODS";
    case DIRECTORY_TOGGLE:
        return "DIRECTORY_TOGGLE";
    case DIRECTORY_FILE:
        return "DIRECTORY_FILE";
    case CGI_EXTENSION:
        return "CGI_EXTENSION";
    case REDIRECT:
        return "REDIRECT";
    }
}

/**
 * @brief HTTPMethod template specialization for strToEnum
 *
 * @param str The string to convert to an enum
 * @return HTTPMethod The HTTPMethod enum value
 */
template <> HTTPMethod strToEnum<HTTPMethod>(const std::string &str)
{
    static const std::string methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OTHER"};

    for (size_t i = 0; i < sizeOfArray(methods); i++)
        if (methods[i] == str)
            return static_cast<HTTPMethod>(i);
    return OTHER;
}

/**
 * @brief ResourceType template specialization for strToEnum
 *
 * @param str The string to convert to an enum
 * @return ResourceType The ResourceType enum value
 */
template <> ResourceType strToEnum<ResourceType>(const std::string &str)
{
    static const std::string requestTypes[] = {"EXISTING_FILE", "REDIRECTION", "FORBIDDEN_METHOD",
                                               "DIRECTORY",     "NOT_FOUND",   "INVALID_REQUEST",
                                               "NO_MATCH"};

    for (size_t i = 0; i < sizeOfArray(requestTypes); i++)
        if (requestTypes[i] == str)
            return static_cast<ResourceType>(i);
    return NOT_FOUND;
}

/**
 * @brief TokenType template specialization for strToEnum
 *
 * @param str The string to convert to an enum
 * @return TokenType The TokenType enum value
 */
template <> TokenType strToEnum<TokenType>(const std::string &str)
{
    static const std::string tokenTypes[] = {"{",
                                             "}",
                                             "#",
                                             ";",
                                             "server",
                                             "listen",
                                             "server_name",
                                             "error_page",
                                             "location",
                                             "try_files",
                                             "body_size",
                                             "methods",
                                             "directory_listing",
                                             "directory_listing_file",
                                             "cgi_extensions",
                                             "redirect"};

    for (size_t i = 0; i < sizeOfArray(tokenTypes); i++)
        if (tokenTypes[i] == str)
            return static_cast<TokenType>(i);
    return WORD;
}
