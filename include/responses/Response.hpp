/**
 * @file Response.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief This class describes an HTTP response for one request
 * @date 2023-07-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

# include <iostream>

class Response
{
    private:
        char *buffer;
        size_t length;
        size_t totalBytesSent;
        int statusCode;
    public:
        Response();
        Response(const Response& r);
        Response& operator=(const Response& r);
        void setResponse(char *newBuf, size_t bufLen);
        void clearResponse();
        ~Response();
};


#endif
