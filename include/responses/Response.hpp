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

#include <ios>
# include <iostream>
# include <sstream>
# include <fstream>
# include "network/network.hpp"

# define IDLE_CONNECTION 0
# define SEND_FAIL 1
# define SEND_PARTIAL 2
# define SEND_SUCCESS 3

// headers
# define STATUS_LINE "HTTP/1.1 "
# define CONTENT_TYPE "Content-Type: "
# define CONTENT_LEN "Content-Length: "
# define KEEP_ALIVE "Connection: keep-alive\r\n"

// content types
# define HTML "text/html; charset=UTF-8\r\n"
# define JPG "image/jpg\r\n"
# define SVG "image/svg+xml\r\n"
# define XML "text/xml\r\n"

// status codes
# define OK_200 "200 OK\r\n"
# define NOTFOUND_404 "404 Not found\r\n"

#define IMG_HEADERS  "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Length: "
#define HTTP_HEADERS "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"

// typedef std::ios_base::streampos f_size;

class Response
{
    private:
        char *_buffer;
        size_t _length;
        size_t _totalBytesSent;
        int _statusCode;

    public:
        Response();
        Response(const Response& r);
        Response& operator=(const Response& r);
        char *buffer();
        size_t length();
        size_t totalBytesSent();
        int statusCode();
        // void setResponse(char *newBuf, size_t bufLen);
        int sendResponse(int fd);
        void createResponse(std::string filename, std::string& headers);
        void createHTMLResponse(std::string page, std::string& headers);
        void clear();
        ~Response();
};

#endif
