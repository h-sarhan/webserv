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

#include <cstddef>
#include <ios>
# include <iostream>
# include <sstream>
# include <fstream>
# include "network/network.hpp"
#include "HeaderData.hpp"
#include "requests/Request.hpp"

# define IDLE_CONNECTION 0
# define SEND_FAIL 1
# define SEND_PARTIAL 2
# define SEND_SUCCESS 3

// headers
# define STATUS_LINE "HTTP/1.1 "
# define CRLF "\r\n"
# define CONTENT_TYPE "Content-Type: "
# define CONTENT_LEN "Content-Length: "
# define LOCATION "Location: "
# define KEEP_ALIVE "Connection: keep-alive"

// content types
# define HTML "text/html; charset=UTF-8"

#define IMG_HEADERS  "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Length: "
#define HTTP_HEADERS "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"

struct Headers
{
    int statusCode;
    std::string contentType;
    size_t contentLen;
    bool keepAlive;
};

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
        void setResponse(std::stringstream& ss);
        void setResponseHeaders(std::stringstream& ss, Headers header);
        void createRedirectResponse(std::string& redirUrl, int statusCode, bool keepAlive);
        void createGETResponse(std::string filename, bool keepAlive);
        void createFileResponse(std::string filename, Request &request, int statusCode);
        void createDELETEResponse(std::string filename, Request &request);
        void createHEADResponse(std::string filename, Request &request);
        void createDirHEADResponse(size_t contentLen, bool keepAlive);
        void createHTMLResponse(int statusCode, std::string page, bool keepAlive);
        void clear();
        ~Response();
};

#endif
