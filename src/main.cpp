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
#include "common.hpp"
#include "network.hpp"
#include <iostream>

/**
 * @brief Entrypoint to our program
 *
 * @return int Exit code
 */
int main(void)
{
    try
    {
        Server s("webserv.com", "1234");
        s.bindSocket();
        s.startListening();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
