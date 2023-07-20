/**
 * @file main.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief The entrypoint to our program
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/Parser.hpp"
#include "network/Server.hpp"
#include "utils.hpp"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

// ! Catch std::bad_alloc and other exceptions here
// ! Clean this up
/**
 * @brief Entrypoint to our program
 *
 * @return int Exit code
 */
int main(int argc, char **argv)
{
    (void) argv;
    if (argc > 2)
    {
        std::cerr << "" << std::endl;
        return (EXIT_FAILURE);
    }

    // Test that validators work
    // Test that request parsing works
    // generateDirectoryListing(".");

    const std::map<std::string, std::string> &keyval = parseKeyValueFile("mime_types.txt", ' ');
    for (std::map<std::string, std::string>::const_iterator it = keyval.begin(); it != keyval.end();
         it++)
    {
        std::cout << "File extension: '" << it->first << "'\t\tMime-type: '" << it->second << "'."
                  << std::endl;
    }
    try
    {
        if (argc == 2)
        {
            std::string filename = argv[1];
            Parser parser(filename);
            std::vector<ServerBlock> &config = parser.getConfig();
            Server s(config);
            s.startListening();
        }
        else
        {
            // uses default config
            std::vector<ServerBlock> config = ServerBlock::createDefaultConfig();
            Server s(config);
            s.startListening();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
