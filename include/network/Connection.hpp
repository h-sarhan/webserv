/**
 * @file Connection.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Class that describes data exchanged in a single connection to the server
 * @date 2023-07-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "network.hpp"
#include <iostream>

class Connection
{
    public:
        std::string request; // this will be an object of its respective class later and not just str
        std::string response; // this will be an object of its respective class later and not just str
        size_t totalBytesRec;
        size_t totalBytesSent;
        Connection();
        ~Connection();
};

#endif
