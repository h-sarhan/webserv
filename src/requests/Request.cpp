/**
 * @file Request.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This class implements the HTTP request class
 * @date 2023-07-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "requests/Request.hpp"
#include "config/Validators.hpp"
#include "enums/conversions.hpp"
#include "network/Server.hpp"
#include "requests/InvalidRequestError.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>

// Implementing resource constructor here
Resource::Resource(const ResourceType &type, const std::string &request, const std::string &path,
                   const Route &route)
    : type(type), originalRequest(request), path(path), route(route)
{
}

/**
 * @brief Construct a new Request object with listener
 *
 * @param listener
 */
Request::Request(int port)
    : _buffer(new char[REQ_BUFFER_SIZE]), _length(0), _capacity(REQ_BUFFER_SIZE), _port(port)
{
}

/**
 * @brief Request copy constructor
 *
 * @param req Request object to copy from
 */
Request::Request(const Request &req)
    : _buffer(new char[req._capacity]), _length(req._length), _capacity(req._capacity),
      _port(req._port)
{
    std::copy(req._buffer, req._buffer + _length, _buffer);
}

/**
 * @brief Request copy assignment constructor
 *
 * @param req Request pbject to copy from
 * @return Request& Instance of own request object
 */
Request &Request::operator=(const Request &req)
{
    if (&req == this)
        return *this;
    _length = req._length;
    _capacity = req._capacity;
    _port = req._port;
    delete[] _buffer;
    _buffer = new char[_capacity];
    std::copy(req._buffer, req._buffer + _length, _buffer);
    return *this;
}

/**
 * @brief Return the HTTP method of the request
 *
 * @return const HTTPMethod& HTTP method of the request
 */
const HTTPMethod &Request::method() const
{
    return _parser.method();
}

/**
 * @brief Return a pointer to the buffer
 *
 * @return const char* A readonly pointer to the buffer
 */
const char *Request::buffer() const
{
    return _buffer;
}

/**
 * @brief Return the amount of bytes recieved from the request
 *
 * @return size_t Amount of bytes in the request
 */
size_t Request::length() const
{
    return _length;
}

/**
 * @brief Return an index to where the body starts
 *
 */
size_t Request::bodyStart() const
{
    return _parser.bodyStart();
}

size_t Request::maxBodySize() const
{
    return _parser.maxBodySize();
}

/**
 * @brief Parse the request
 *
 * @return true if the request has been processed completely
 * @return false if more bytes are required to process the request
 */
bool Request::parseRequest()
{
    // Check if the buffer is empty or if the request has already been processed
    if (_length == 0)
        return false;

    return _parser.parse(_buffer, _length, Server::getConfig(_port));
}

/**
 * @brief Get a reference to the parsed headers
 *
 * @return std::map<std::string, const std::string>& Reference to parsed headers
 */
std::map<std::string, std::string> &Request::headers()
{
    return _parser.headers();
}

/**
 * @brief Get the keep alive value
 *
 * @return bool Whether to keep the connection alive
 */
bool Request::keepAlive() const
{
    return _parser.keepAlive().first;
}

/**
 * @brief Amount int seconds to keep the connection alive for
 *
 * @return unsigned int How long to keep the connection alive for
 */
unsigned int Request::keepAliveTimer() const
{
    return _parser.keepAlive().second;
}

/**
 * @brief Get the resource that was requested by the client
 *
 * @param config Configuration to scan through
 * @return const Resource The resource requested
 */
const Resource &Request::resource() const
{
    return _parser.resource();
}

static const char *getLineEnd(const char *start, const char *end)
{
    static const char *crlf = "\r\n";
    return std::search(start, end, crlf, crlf + 2);
}

/**
 * @brief Unchunks a request encoded in chunked transfer encoding
 *
 */
void Request::unchunk()
{
    const char *pos = _buffer + _parser.bodyStart();
    const char *reqEnd = _buffer + _length;
    unsigned int newLength = _parser.bodyStart();

    // Read chunk size
    const char *lineEnd = getLineEnd(pos, reqEnd);
    if (lineEnd == reqEnd)
        return;

    // Allocate for unchunked request and copy start line and headers
    char *unchunkedRequest = new char[_capacity + 1];
    std::copy(_buffer, _buffer + _parser.bodyStart(), unchunkedRequest);

    unsigned int chunkLen = getHex(std::string(pos, lineEnd));
    pos = lineEnd + 2;
    while (chunkLen > 0)
    {
        lineEnd = pos + chunkLen;
        if (lineEnd == reqEnd || newLength > _length || lineEnd[0] != '\r' || lineEnd[1] != '\n')
        {
            delete[] unchunkedRequest;
            return;
        }
        std::copy(pos, lineEnd, unchunkedRequest + newLength);
        newLength += chunkLen;
        pos = lineEnd + 2;
        lineEnd = getLineEnd(pos, reqEnd);
        if (lineEnd == reqEnd)
        {
            delete[] unchunkedRequest;
            return;
        }
        chunkLen = getHex(std::string(pos, lineEnd));
        pos = lineEnd + 2;
    }
    delete[] _buffer;
    unchunkedRequest[newLength] = '\0';
    _length = newLength;
    _buffer = unchunkedRequest;
}

/**
 * @brief Resize buffer to new capacity
 *
 * @param newCapacity The new buffer capacity
 */
void Request::resizeBuffer(size_t newCapacity)
{
    char *newBuffer = new char[newCapacity];
    std::copy(_buffer, _buffer + _length, newBuffer);
    delete[] _buffer;
    _buffer = newBuffer;
    _capacity = newCapacity;
}

/**
 * @brief Append new request data to buffer, resizing if necessary
 *
 * @param data New request data
 * @param n Amount of bytes to add
 */
void Request::appendToBuffer(const char *data, const size_t n)
{
    if (_length + n >= _capacity)
        resizeBuffer(std::max(_capacity * 2, (_length + n) * 2));
    std::copy(data, data + n, _buffer + _length);
    _length += n;
}

/**
 * @brief Clear properties of the request
 */
void Request::clear()
{
    _length = 0;
    _parser.clear();
}

/**
 * @brief Destroy the Request object
 */
Request::~Request()
{
    delete[] _buffer;
}
