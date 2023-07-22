/**
 * @file TokenTypes.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the tokens types that will be used by the tokenizer
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef TOKEN_TYPES_HPP
#define TOKEN_TYPES_HPP

/**
 * @brief The types of tokens that will be produced after tokenizing the config file
 */
typedef enum
{
    // Single-character tokens.
    LEFT_BRACE,
    RIGHT_BRACE,
    POUND,
    SEMICOLON,

    // Keywords.
    SERVER,
    LISTEN,
    SERVER_NAME,
    ERROR_PAGE,
    LOCATION,
    TRY_FILES,
    BODY_SIZE,
    METHODS,
    AUTO_INDEX,
    INDEX,
    CGI_EXTENSION,
    RETURN,

    // Literals.
    WORD
} TokenType;

#endif
