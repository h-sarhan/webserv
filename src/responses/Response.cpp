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

void Response::createRedirectResponse(std::string& redirUrl, int statusCode, bool keepAlive)
{
    std::stringstream responseBuffer;

    responseBuffer << STATUS_LINE << getStatus(statusCode) << CRLF;
    responseBuffer << LOCATION << redirUrl << CRLF;
    if (keepAlive)
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << CONTENT_LEN << "0" << CRLF;
    responseBuffer << CRLF;
    _length = getStreamLen(responseBuffer);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::createGETResponse(std::string filename, bool keepAlive)
{
    std::ifstream file;
    std::stringstream responseBuffer;
    std::string mimeType;

    file.open(filename.c_str(), std::ios::binary);
    if (!file.good())
        return createHTMLResponse(404, errorPage(404), false);
    // mimeType = getMimeType(filename);
    mimeType = "text/html; charset=UTF-8";

    size_t fileLen = getStreamLen(file);

    responseBuffer << STATUS_LINE << getStatus(200) << CRLF;
    responseBuffer << CONTENT_TYPE << mimeType << CRLF;
    responseBuffer << CONTENT_LEN << fileLen << CRLF;
    if (keepAlive)
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << CRLF;
    responseBuffer << file.rdbuf();
    file.close();

    _length = getStreamLen(responseBuffer);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::createPOSTResponse(std::string filename, Request& request)
{
    std::stringstream responseBuffer;
    std::ofstream file;
    std::string path("./posts" + filename);
    std::cout << "path is " << path << std::endl;
    file.open(path);
    if (!file.good())
    {
        std::cout << "Cannot open file to write" << std::endl;
        return createHTMLResponse(500, errorPage(500), false);
    }
    file.write(request.buffer() + request.bodyStart(), request.requestLength() - request.bodyStart());
    file.close();
    responseBuffer << STATUS_LINE << getStatus(201) << CRLF;
    if (request.keepAlive())
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << LOCATION << path << CRLF;
    responseBuffer << CRLF;
    _length = getStreamLen(responseBuffer);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::createPUTResponse(std::string filename, Request& request)
{
    std::stringstream responseBuffer;
    std::ofstream file;
    std::string path("./posts" + filename);
    std::cout << "path is " << path << std::endl;
    file.open(path);
    if (!file.good())
    {
        std::cout << "Cannot open file to write" << std::endl;
        return createHTMLResponse(500, errorPage(500), false);
    }
    file.write(request.buffer() + request.bodyStart(), request.requestLength() - request.bodyStart());
    file.close();
    responseBuffer << STATUS_LINE << getStatus(201) << CRLF;
    if (request.keepAlive())
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << LOCATION << path << CRLF;
    responseBuffer << CRLF;
    _length = getStreamLen(responseBuffer);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::createHTMLResponse(int statusCode, std::string page, bool keepAlive)
{
    std::stringstream responseBuffer;

    responseBuffer << STATUS_LINE << getStatus(statusCode) << CRLF;
    if (keepAlive)
        responseBuffer << KEEP_ALIVE << CRLF;
    // responseBuffer << CONTENT_TYPE << getContentType(extension) << CRLF;
    responseBuffer << CONTENT_TYPE << HTML << CRLF;
    responseBuffer << CONTENT_LEN << page.length() << CRLF << CRLF;
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
