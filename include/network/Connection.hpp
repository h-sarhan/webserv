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
#include "logger/Logger.hpp"

class Connection : Logger
{
    using Logger::log;
    private:
        void showResourceInfo(Resource &resource);
        void processGET();
        void processPOST();
        void processPUT();
        void processDELETE();
        void processHEAD();

    public:
        int listener; // this is the server socket through which this connection was created - not to be confused with the new client fd
        Request request; // this will be an object of its respective class later and not just str
        Response response; // this will be an object of its respective class later and not just str
        bool keepAlive;
        time_t timeOut;
        time_t startTime;
        // Logger& log;

        Connection();
        Connection(int listener);
        Connection(const Connection &c);
        Connection& operator=(const Connection &c);
        void processRequest();
        bool keepConnectionAlive();
        ~Connection();

};

#endif
