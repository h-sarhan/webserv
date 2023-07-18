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
#include "responses/DefaultPages.hpp"
#include <ios>

Response::Response() : _buffer(NULL), _length(0), _totalBytesSent(0), _statusCode(0)
{

}

Response::Response(const Response& r) : _buffer(NULL), _length(r._length), _totalBytesSent(r._totalBytesSent), _statusCode(r._statusCode)
{
    if (this->_buffer != NULL)
        delete[] _buffer;
    this->_buffer = new char[r._length];
    for (size_t i = 0; i < r._length; i++)
        this->_buffer[i] = r._buffer[i];
}

Response& Response::operator=(const Response& r)
{
    if (this != &r)
    {
        if (this->_buffer != NULL)
            delete[] _buffer;
        this->_buffer = new char[r._length];
        for (size_t i = 0; i < r._length; i++)
            this->_buffer[i] = r._buffer[i];
        this->_length = r._length;
        this->_totalBytesSent = r._totalBytesSent;
        this->_statusCode = r._statusCode;
    }
    return (*this);
}

char* Response::buffer()
{
    return _buffer;
}

size_t Response::length()
{
    return _length;
}

size_t Response::totalBytesSent()
{
    return _totalBytesSent;
}

int Response::statusCode()
{
    return _statusCode;
}


void Response::setResponse(char *newBuf, size_t bufLen)
{
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[bufLen];
    for (size_t i = 0; i < _length; i++)
        _buffer[i] = newBuf[i];
}

void Response::clearResponse()
{
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = NULL;
    _length = 0;
    _totalBytesSent = 0;
    _statusCode = 0;
}

template <typename streamType>
static size_t getStreamLen(streamType &s)
{
    size_t len;

    s.seekg(0, std::ios::end);
    len = s.tellg();
    s.seekg(0, std::ios::beg);

    return len;
}

void Response::createResponse(std::string filename, std::string headers)
{
    std::ifstream file;
    std::stringstream responseBuffer;
    std::string fileContents;

    file.open(filename.c_str(), std::ios::binary);
    if (!file.good())
        return createHTMLResponse(errorPage(404), headers);

    size_t fileLen = getStreamLen(file);

    responseBuffer << headers;
    responseBuffer << fileLen << "\r\n\r\n";
    responseBuffer << file.rdbuf();
    file.close();
    
    _length = getStreamLen(responseBuffer);
    
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::createHTMLResponse(std::string page, std::string headers)
{
    std::stringstream responseBuffer;

    responseBuffer << headers;
    responseBuffer << page.length() << "\r\n\r\n";
    responseBuffer << page;
    _length = getStreamLen(responseBuffer);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

Response::~Response()
{
    if (_buffer != NULL)
        delete[] _buffer;
}
