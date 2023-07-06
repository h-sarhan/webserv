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

typedef std::istream_iterator<std::string> tokenIterator;

// TODO: Create exception class for config file not found
/**
 * @brief This class tokenizes the configuration file of our web server.
 * The list of tokens produces helps with parsing the configuration.
 */
class Tokenizer
{
  private:
    std::vector<Token> _tkns;
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
    const std::vector<Token> &tokens(void) const;

  private:
    Tokenizer(const Tokenizer &old);
    Tokenizer &operator=(const Tokenizer &old);
    void tokenizeFile(std::ifstream &configFile);
    void tokenizeLine(const std::string &line, const uint32_t num);
};
#endif
