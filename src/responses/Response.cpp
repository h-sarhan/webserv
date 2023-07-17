/**
 * @file Response.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of Response class
 * @date 2023-07-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "responses/Response.hpp"

Response::Response() : buffer(NULL), length(0), totalBytesSent(0), statusCode(0)
{

}

Response::Response(const Response& r) : length(r.length), totalBytesSent(r.totalBytesSent), statusCode(r.statusCode)
{
    if (this->buffer != NULL)
        delete[] buffer;
    this->buffer = new char[r.length];
    for (size_t i = 0; i < r.length; i++)
        this->buffer[i] = r.buffer[i];
}

Response& Response::operator=(const Response& r)
{
    if (this != &r)
    {
        if (this->buffer != NULL)
            delete[] buffer;
        this->buffer = new char[r.length];
        for (size_t i = 0; i < r.length; i++)
            this->buffer[i] = r.buffer[i];
        this->length = r.length;
        this->totalBytesSent = r.totalBytesSent;
        this->statusCode = r.statusCode;
    }
    return (*this);
}

void Response::setResponse(char *newBuf, size_t bufLen)
{
    if (buffer != NULL)
        delete[] buffer;
    buffer = new char[bufLen];
    for (size_t i = 0; i < length; i++)
        buffer[i] = newBuf[i];
}

void Response::clearResponse()
{
    if (buffer != NULL)
        delete[] buffer;
    buffer = NULL;
    length = 0;
    totalBytesSent = 0;
    statusCode = 0;
}


Response::~Response()
{
    if (buffer != NULL)
        delete[] buffer;
}
