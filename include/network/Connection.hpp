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

using logger::Log;

class Connection
{
    private:
        int _listener; // this is the server socket through which this connection was created - not to be confused with the new client fd
        Request _request; // this will be an object of its respective class later and not just str
        Response _response; // this will be an object of its respective class later and not just str
        bool _keepAlive;
        time_t _timeOut;
        time_t _startTime;
        bool _dropped;
        std::string _ip;
        bool _reqReady; // whether the request is ready to be processed - set to true when the request is fully received and false when the response is fully sent

        void processGET();
        void processPOST();
        void processPUT();
        void processDELETE();
        void processHEAD();

    public:
        Connection();
        Connection(int listener, std::string ip);
        Connection(const Connection &c);
        Connection& operator=(const Connection &c);
        int& listener();
        Request& request();
        Response& response();
        bool& keepAlive();
        time_t& timeOut();
        time_t& startTime();
        bool& dropped();
        bool& reqReady();
        std::string ip();
        void processRequest();
        bool keepConnectionAlive();
        bool bodySizeExceeded();
        std::vector<char *> prepCGIEnvironment();
        ~Connection();

};

#endif
