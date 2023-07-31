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
# include "logger/Logger.hpp"
# include "network/network.hpp"
#include "HeaderData.hpp"
#include "requests/Request.hpp"
#include <sys/wait.h>

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
# define NO_CONTENT ""

struct Headers
{
    int statusCode;
    std::string contentType;
    size_t contentLen;
    bool keepAlive;
};

using logger::Log;
class Response
{
    // using Logger::log;
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
        
        int sendResponse(int fd);
        void setResponse(std::stringstream& ss);
        void setResponseHeaders(std::stringstream& ss, Headers header);
        void createRedirectResponse(std::string& redirUrl, int statusCode, bool keepAlive);
        void createGETResponse(Request &request);
        void createFileResponse(Request &request, int statusCode);
        void createDELETEResponse(Request &request);
        void createHEADFileResponse(Request &request);
        void createHEADResponse(int statusCode, std::string contentType, bool keepAlive);
        void createHTMLResponse(int statusCode, std::string page, bool keepAlive);
        void trimBody();

        // CGI
        int sendCGIRequestBody(int pipeFd, Request &req);
        void readCGIResponse(int pipeFd, Request &req);
        void runCGI(int p[2][2], Request &req, std::vector<char *> env);
        void createCGIResponse(Request &request, std::vector<char *> env);

        void clear();
        ~Response();
};

#endif
