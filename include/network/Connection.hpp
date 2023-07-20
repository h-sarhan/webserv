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
#include "requests/Request.hpp"
#include "responses/Response.hpp"
#include <iostream>
// #include <ctime>

typedef std::vector<ServerBlock *>& configList;

class Connection
{
    public:
        int listener; // this is the server socket through which this connection was created - not to be confused with the new client fd
        Request request; // this will be an object of its respective class later and not just str
        Response response; // this will be an object of its respective class later and not just str

        bool keepAlive;
        time_t timeOut;
        time_t startTime;

        Connection();
        Connection(int listener);
        Connection(const Connection &c);
        Connection& operator=(const Connection &c);
        void processRequest(configList config);
        void processGET(configList config);
        void processPOST(configList config);
        void processPUT(configList config);
        void processDELETE(configList config);
        void processHEAD(configList config);

        bool keepConnectionAlive();
        std::string createResponseHeaders();
        ~Connection();

};

#endif
