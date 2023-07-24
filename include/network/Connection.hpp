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

using logger::log;
class Connection
{
    private:
        int _listener; // this is the server socket through which this connection was created - not to be confused with the new client fd
        Request _request; // this will be an object of its respective class later and not just str
        Response _response; // this will be an object of its respective class later and not just str
        bool _keepAlive;
        time_t _timeOut;
        time_t _startTime;

        void processGET();
        void processPOST();
        void processPUT();
        void processDELETE();
        void processHEAD();

    public:
        Connection();
        Connection(int listener);
        Connection(const Connection &c);
        Connection& operator=(const Connection &c);
        int& listener();
        Request& request();
        Response& response();
        bool& keepAlive();
        time_t& timeOut();
        time_t& startTime();
        void processRequest();
        bool keepConnectionAlive();
        ~Connection();

};

#endif
