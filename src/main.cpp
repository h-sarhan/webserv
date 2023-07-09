/**
 * @file main.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief The entrypoint to our program
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <vector>

// /**
//  * @brief Static helper function to print a token
//  *
//  * @param token Token to be printed
//  */
// static void printToken(const Token &token)
// {
//     std::cout << "|" << token << "|" << std::endl;
// }

/**
 * @brief Entrypoint to our program
 *
 * @return int Exit code
 */
int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cerr << "" << std::endl;
        return (EXIT_FAILURE);
    }
    std::string filename = "./example.conf";
    if (argc == 2)
        filename = argv[1];
    try
    {
        ConfigTokenizer tokenizer(filename);
        std::vector<Token> tokens = tokenizer.tokens();

        Server s("webserv.com", "1234");
        s.bindSocket();
        s.startListening();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
