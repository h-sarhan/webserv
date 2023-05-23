/**
 * @file CofigTokens.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the tokens that we will be used while parsing the
 * confg file
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * @brief The tokens that will be produced after tokenizing the config file
 *
 */
enum ConfigTokenType
{
    // Single-character tokens.
    LEFT_BRACE,
    RIGHT_BRACE,
    POUND,
    SEMICOLON,

    // Literals.
    WORD,
    NUMBER,

    // Keywords.
    SERVER,
    LISTEN,
    SERVER_NAME,
    ERROR_PAGE,
    LOCATION,
    TRY_FILES,
    BODY_SIZE,
    METHODS,
    DIRECTORY_TOGGLE,
    DIRECTORY_FILE,
    CGI_EXTENSION,
    REDIRECT,

    EOF
};
