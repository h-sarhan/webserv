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

  public:
    // Creates a Tokenizer object with the config file that it is tokenizing
    Tokenizer(const std::string &filename);
    ~Tokenizer();

    // Return the produced tokens
    const std::vector<Token> &tokens() const;

  private:
    // Copy constructors have been made private since this class will never be copied
    Tokenizer(const Tokenizer &old);
    Tokenizer &operator=(const Tokenizer &old);

    // Tokenization functions
    void tokenizeFile(std::ifstream &configStream);
    void tokenizeLine(std::string &lineStr, const unsigned int lineNum);
    void tokenizeWord(const std::string &wordStr, const unsigned int wordPos,
                      const unsigned int lineNum);

    // Check if the provided character is one of the single character tokens
    bool isSingleCharToken(const char c) const;

    // Add a token to the list of tokens
    void addWordToken(std::string::const_iterator &wordIt, const std::string &wordStr,
                      const unsigned int lineNum, const unsigned int column);
};
#endif
