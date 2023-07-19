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

Response::Response() : _buffer(NULL), _length(0), _totalBytesSent(0), _statusCode(0)
{
}

Response::Response(const Response &r)
    : _buffer(NULL), _length(r._length), _totalBytesSent(r._totalBytesSent),
      _statusCode(r._statusCode)
{
    if (this->_buffer != NULL)
        delete[] _buffer;
    this->_buffer = new char[r._length];
    for (size_t i = 0; i < r._length; i++)
        this->_buffer[i] = r._buffer[i];
}

Response &Response::operator=(const Response &r)
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

char *Response::buffer()
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

// void Response::setResponse(char *newBuf, size_t bufLen)
// {
//     if (_buffer != NULL)
//         delete[] _buffer;
//     _buffer = new char[bufLen];
//     for (size_t i = 0; i < _length; i++)
//         _buffer[i] = newBuf[i];
// }

void Response::clear()
{
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = NULL;
    _length = 0;
    _totalBytesSent = 0;
    _statusCode = 0;
}

int Response::sendResponse(int fd)
{
    ssize_t bytesSent;

    if (_length == 0)
    {
        // std::cout << "Connection is idle: " << fd << std::endl;
        return IDLE_CONNECTION;
    }
    std::cout << "Sending a response... " << std::endl;
    bytesSent = send(fd, _buffer + _totalBytesSent, _length - _totalBytesSent, 0);
    if (bytesSent < 0)
    {
        std::cout << "Sending response failed: " << strerror(errno) << std::endl;
        return SEND_FAIL;
    }
    else
    {
        _totalBytesSent += bytesSent;
        if (_totalBytesSent < _length)   // partial send
        {
            std::cout << "Response only sent partially: " << bytesSent
                      << ". Total: " << _totalBytesSent << std::endl;
            return SEND_PARTIAL;
        }
    }
    std::cout << "Response sent successfully to fd " << fd
              << ", total bytes sent = " << _totalBytesSent << std::endl;
    return SEND_SUCCESS;
}

template <typename streamType> static size_t getStreamLen(streamType &s)
{
    size_t len;

    s.seekg(0, std::ios::end);
    len = s.tellg();
    s.seekg(0, std::ios::beg);

    return len;
}

void Response::createResponse(std::string filename, std::string& headers)
{
    std::string headers;
    std::ifstream file;
    std::stringstream responseBuffer;
    std::string fileContents;

    file.open(filename.c_str(), std::ios::binary);
    if (!file.good())
        return createHTMLResponse(errorPage(404), headers);

    size_t fileLen = getStreamLen(file);

    responseBuffer << headers;
    responseBuffer << CONTENT_LEN << fileLen << "\r\n\r\n";
    responseBuffer << file.rdbuf();
    file.close();

    _length = getStreamLen(responseBuffer);

    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::createHTMLResponse(std::string page, std::string& headers)
{
    std::stringstream responseBuffer;

    responseBuffer << headers;
    responseBuffer << CONTENT_LEN << page.length() << "\r\n\r\n";
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
