/**
 * @file Tokenizer.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the Tokenizer class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Token.hpp"
#include "common.hpp"
#include <map>

typedef std::istream_iterator<std::string> tokenIterator;

// TODO: Create exception class for config file not found
// TODO: Create exception class for unexpected token
/**
 * @brief This class tokenizes the configuration file of our web server.
 * The list of tokens produces helps with parsing the configuration.
 * Note: The tokenization is based on whitespace.
 */
class Tokenizer
{
  private:
    std::vector<const Token> _tkns;
    uint32_t _line;

  public:
    /**
     * @brief Construct a new Tokenizer object
     *
     * @param filename The path to the config file
     */
    Tokenizer(const std::string &filename);

    /**
     * @brief Returns the token list generated
     *
     * @return const std::vector<Token>& The token list
     */
    const std::vector<const Token> &tokens(void) const;

    static std::map<const std::string, const TokenType> strToToken;
    static std::map<const TokenType, const std::string> tokenToStr;

  private:
    Tokenizer(const Tokenizer &old);
    Tokenizer &operator=(const Tokenizer &old);
    void tokenizeFile(std::ifstream &configFile);
    void tokenizeLine(std::string line, const uint32_t num);
    void tokenizeStr(const std::string &contents, const uint32_t start,
                     const uint32_t lineNum);
    bool isSingleCharToken(const char c) const;
    void addSingleCharToken(const char tokenChr, const uint32_t line,
                            const uint32_t column);
    void addWord(uint32_t &i, const std::string &contents,
                 const uint32_t lineNum, const uint32_t column);
};
#endif
