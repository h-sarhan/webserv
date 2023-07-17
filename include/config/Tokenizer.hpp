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
#include <iterator>
#include <map>
#include <vector>

typedef std::istream_iterator<std::string> tokenIterator;

/**
 * @brief This class tokenizes the configuration file of our web server.
 * The list of tokens produces helps with parsing the configuration file.
 */
class Tokenizer
{
  private:
    std::vector<Token> _tokens;
    unsigned int _line;

  public:
    Tokenizer(const std::string &filename);

    ~Tokenizer();

    const std::vector<Token> &tokens() const;

  private:
    Tokenizer(const Tokenizer &old);
    Tokenizer &operator=(const Tokenizer &old);

    void tokenizeFile(std::ifstream &configStream);
    void tokenizeLine(std::string &lineStr, const unsigned int lineNum);
    void tokenizeWord(const std::string &wordStr, const unsigned int wordPos,
                      const unsigned int lineNum);
    bool isSingleCharToken(const char c) const;
    void addWord(unsigned int &wordIdx, const std::string &wordStr, const unsigned int lineNum,
                 const unsigned int column);
};
#endif
