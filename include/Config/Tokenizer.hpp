/**
 * @file Tokenizer.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the ConfigTokenizer class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Token.hpp"
#include "common.hpp"

typedef std::istream_iterator<std::string> tokenIterator;

/**
 * @brief This class tokenizes the configuration file of our web server.
 * The list of tokens produces helps with parsing the configuration file.
 */
class ConfigTokenizer
{
  private:
    std::vector<const Token> _tokens;
    uint32_t _line;

  public:
    ConfigTokenizer(const std::string &filename);

    ~ConfigTokenizer(void);

    const std::vector<const Token> &tokens(void) const;

    // Map strings to tokens
    static std::map<const std::string, const TokenType> strToToken;

    // Maps tokens to strings
    static std::map<const TokenType, const std::string> tokenToStr;

  private:
    ConfigTokenizer(const ConfigTokenizer &old);
    ConfigTokenizer &operator=(const ConfigTokenizer &old);

    void tokenizeFile(std::ifstream &configStream);
    void tokenizeLine(std::string &lineStr, const uint32_t lineNum);
    void tokenizeWord(const std::string &wordStr, const uint32_t wordPos,
                      const uint32_t lineNum);
    bool isSingleCharToken(const char c) const;
    void addWord(uint32_t &wordIdx, const std::string &wordStr,
                 const uint32_t lineNum, const uint32_t column);
};
#endif
