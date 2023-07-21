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
#include "requests/InvalidRequestError.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

/**
 * @brief Construct a new Request object
 */
Request::Request()
    : _httpMethod(), _requestedURL(), _headers(), _buffer(new char[REQ_BUFFER_SIZE]), _length(0),
      _capacity(REQ_BUFFER_SIZE), _bodyStart(0), _valid(true)
{
}

/**
 * @brief Request copy constructor
 *
 * @param req Request object to copy from
 */
Request::Request(const Request &req)
    : _httpMethod(req.method()), _requestedURL(req._requestedURL), _headers(req._headers),
      _buffer(new char[req._capacity]), _length(req._length), _capacity(req._capacity),
      _bodyStart(req._bodyStart), _valid(req._valid)
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
    _httpMethod = req._httpMethod;
    _requestedURL = req._requestedURL;
    _headers = req._headers;
    _length = req._length;
    _capacity = req._capacity;
    _valid = req._valid;
    _bodyStart = req._bodyStart;
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
    return _httpMethod;
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
size_t Request::requestLength() const
{
    return _length;
}

size_t Request::bodyStart() const
{
    return _bodyStart;
}

// std::string Request::rawTarget()
// {
//     return _requestedURL;
// }

/**
 * @brief Check that a condition is true, else throw an InvalidRequestError exception
 *
 * @param condition Condition to check
 * @param throwMsg Error message
 */
void Request::assertThat(bool condition, const std::string &throwMsg) const
{
    if (!condition)
        throw InvalidRequestError(throwMsg);
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
    if (!_headers.empty())
        return true;

    // Search for double CRLF in the request buffer, return false if not recieved
    const char doubleCRLF[] = "\r\n\r\n";
    char *bodyStart = std::search(_buffer, _buffer + _length, doubleCRLF,
                                  doubleCRLF + sizeOfArray(doubleCRLF) - 1);
    if (bodyStart == _buffer + _length)
        return false;
    _bodyStart = bodyStart - _buffer + 4;
    // If double CRLF is found then we form a stringstream starting from the beginning of the
    // request till the last header
    const std::string requestHead(_buffer, bodyStart + 2);
    std::stringstream requestStream(requestHead);

    try
    {
        parseStartLine(requestStream);
        // Parse headers
        while (requestStream.peek() != '\r' && !requestStream.eof())
            parseHeader(requestStream);
    }
    catch (const InvalidRequestError &e)
    {
        // ! Log here using the logger class that we have recieved an invalid request
        std::cerr << e.what() << std::endl;
        _valid = false;
    }
    return true;
}

/**
 * @brief Parses the start line of HTTP request.
 * The start line usually looks like:
 * METHOD RESOURCE HTTP/VERSION\r\n
 * Example:
 * GET /background.png HTTP/1.0
 *
 * @param reqStream The stream containing the request start line
 */
void Request::parseStartLine(std::stringstream &reqStream)
{
    // Parse HTTP method as a string
    const std::string &httpMethod = getNext<std::string>(reqStream);
    assertThat(!httpMethod.empty(), "Invalid HTTP method in start line");

    // Convert HTTP method to an enum
    _httpMethod = strToEnum<HTTPMethod>(httpMethod);
    assertThat(_httpMethod != OTHER, "Invalid HTTP method in start line");

    // Parse the requested resource path
    _requestedURL = getNext<std::string>(reqStream);
    assertThat(!_requestedURL.empty(), "Invalid resource in start line");

    // Clean the resource URL
    sanitizeURL(_requestedURL);

    // Check HTTP version
    const std::string &httpVersion = getNext<std::string>(reqStream);
    assertThat(httpVersion == "HTTP/1.0" || httpVersion == "HTTP/1.1",
               "Invalid/unsupported HTTP verion in start line");

    // Check that the start line ends with CRLF
    checkLineEnding(reqStream);
}

/**
 * @brief Checks that the next two characters are CRLF
 *
 * @param reqStream The stream containing the request
 */
void Request::checkLineEnding(std::stringstream &reqStream) const
{
    reqStream >> std::noskipws;
    const char cr = getNext<char>(reqStream);
    const char lf = getNext<char>(reqStream);
    reqStream >> std::skipws;
    assertThat(cr == '\r' && lf == '\n', "Line does not end in CRLF");
}

/**
 * @brief Parse a header field from the HTTP request
 * A header field in an HTTP request looks like this:
 * key: value\r\n
 * Example:
 * Host: webserv.com:80
 *
 * @param reqStream The stream containing the Header field
 */
void Request::parseHeader(std::stringstream &reqStream)
{
    // Get the key from the header field
    std::string key = getNext<std::string>(reqStream);
    assertThat(key.length() >= 2, "Invalid header");

    // Check that the colon separator is there
    const char sep = key[key.length() - 1];
    assertThat(sep == ':', "Invalid header");

    // Remove colon separator
    rightTrimStr(key, ":");

    // Get the value. We read character by character here because whitespace is included
    std::string value;
    reqStream >> std::noskipws;
    while (reqStream.peek() != '\r' && !reqStream.eof())
        value.push_back(getNext<char>(reqStream));
    assertThat(!value.empty() && !reqStream.eof(), "Invalid header");

    // Trim value of whitespace
    trimStr(value, WHITESPACE);

    // Transform value to lowercase
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    // Check that the line ends in CRLF
    checkLineEnding(reqStream);

    // Insert header field into map
    _headers.insert(std::make_pair(key, value));
}

/**
 * @brief Get the specified hostname from the Request
 *
 * @return const std::string Requested hostname
 */
const std::string Request::hostname() const
{
    if (_headers.count("host") == 0)
        return DEFAULT_HOSTNAME;

    std::string hostValue = _headers.at("host");
    hostValue = hostValue.substr(0, hostValue.find(":"));

    // Check if the host value is a port rather than a hostname
    const size_t numDigits = std::count_if(hostValue.begin(), hostValue.end(), ::isdigit);
    if (numDigits == hostValue.length())
        return DEFAULT_HOSTNAME;

    if (validateHostName(hostValue))
    {
        // ! Log here that an invalid hostname is in the header
        return hostValue;
    }
    return DEFAULT_HOSTNAME;
}

/**
 * @brief Get a reference to the parsed headers
 *
 * @return std::map<std::string, const std::string>& Reference to parsed headers
 */
std::map<std::string, const std::string> &Request::headers()
{
    return _headers;
}

/**
 * @brief Get the keep alive value
 *
 * @return bool Whether to keep the connection alive
 */
bool Request::keepAlive() const
{
    return _headers.count("connection") == 0 || _headers.at("connection") != "closed";
}

/**
 * @brief Amount int seconds to keep the connection alive for
 *
 * @return unsigned int How long to keep the connection alive for
 */
unsigned int Request::keepAliveTimer() const
{
    if (!keepAlive())
        return 0;

    if (_headers.count("keep-alive") == 0)
        return DEFAULT_KEEP_ALIVE_TIME;

    const std::string &keepAliveValue = _headers.at("keep-alive");
    if (keepAliveValue.find("timeout=") == std::string::npos)
        return DEFAULT_KEEP_ALIVE_TIME;

    std::stringstream timeOutStream(keepAliveValue.substr(keepAliveValue.find("timeout=")));
    getNext<std::string>(timeOutStream);
    unsigned int timeout = getNext<unsigned int>(timeOutStream);
    if (timeOutStream.good() && timeout <= MAX_KEEP_ALIVE_TIME)
        return timeout;
    return DEFAULT_KEEP_ALIVE_TIME;
}

/**
 * @brief Max reconnections requested by client
 *
 * @return unsigned int How many requests to recieve from client before dropping connection
 */
unsigned int Request::maxReconnections() const
{
    if (!keepAlive())
        return 1;

    if (_headers.count("keep-alive") == 0)
        return DEFAULT_RECONNECTIONS;
    const std::string &keepAliveValue = _headers.at("keep-alive");

    std::stringstream maxConnStream(keepAliveValue.substr(keepAliveValue.find("max=")));
    getNext<std::string>(maxConnStream);
    const unsigned int maxConnections = getNext<unsigned int>(maxConnStream);
    if (maxConnStream.good() && maxConnections <= MAX_RECONNECTIONS)
        return maxConnections;
    return DEFAULT_RECONNECTIONS;
}

/**
 * @brief Returns the size of the body based on the Content-Length header
 *
 * @return size_t Size of body in bytes
 */
size_t Request::bodySize() const
{
    if (_headers.count("content-length") == 0)
        return -1;
    return fromStr<size_t>(_headers.at("content-length"));
}

// ! This can be refactored into a function object that is passed to std::find()
const std::map<std::string, Route>::const_iterator Request::getMatchingRoute(
    const std::map<std::string, Route> &routes) const
{
    // std::map<std::string, Route>::const_iterator routeIt = routes.end();
    std::map<std::string, Route>::const_iterator routeIt = routes.end();
    for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); it++)
    {
        // Two conditions need to be true in order for matchedLocation to equal it->first
        // 1. _requestedURL has to start with it->first
        // 2. it->first has to be greater than matchedLocation.length()
        if (_requestedURL.length() >= it->first.length() &&
            std::equal(it->first.begin(), it->first.end(), _requestedURL.begin()))
        {
            if (routeIt == routes.end())
                routeIt = it;
            else if (it->first.length() > routeIt->first.length())
                routeIt = it;
        }
    }
    return routeIt;
}

std::string Request::formPathToResource(const std::pair<std::string, Route> &route) const
{
    std::string resourcePath =
        route.second.serveDir + "/" +
        _requestedURL.substr(_requestedURL.find(route.first) + route.first.length());
    removeDuplicateChar(resourcePath, '/');
    return resourcePath;
}

const Resource Request::getResourceFromConfig(const std::map<std::string, Route> &routes) const
{
    const std::map<std::string, Route>::const_iterator &routeIt = getMatchingRoute(routes);
    if (routeIt == routes.end())
        return Resource(NO_MATCH, _requestedURL);

    const Route &routeOptions = routeIt->second;
    if (routeOptions.methodsAllowed.count(_httpMethod) == 0)
        return Resource(FORBIDDEN_METHOD, _requestedURL);
    if (routeOptions.redirectTo.length() > 0)
        return Resource(REDIRECTION, _requestedURL, routeOptions.redirectTo, routeOptions);
    if (routeOptions.serveDir.length() == 0)
        return Resource(NOT_FOUND, _requestedURL, _requestedURL, routeOptions);

    const std::string &resourcePath = formPathToResource(*routeIt);
    // Check if the resource exists
    struct stat info;
    if (stat(resourcePath.c_str(), &info) != 0)
        return Resource(NOT_FOUND, _requestedURL, resourcePath, routeOptions);

    // Check if the resource is a file or a directory
    const bool isFile = info.st_mode & S_IFREG;
    const bool isDir = info.st_mode & S_IFDIR;
    if (isFile)
        return Resource(EXISTING_FILE, _requestedURL, resourcePath, routeOptions);
    if (isDir && routeOptions.listDirectories == true)
        return Resource(DIRECTORY, _requestedURL, resourcePath, routeOptions);
    if (isDir && !routeOptions.listDirectoriesFile.empty())
        return Resource(EXISTING_FILE, _requestedURL, routeOptions.listDirectoriesFile,
                        routeOptions);
    return Resource(NOT_FOUND, _requestedURL, resourcePath, routeOptions);
}

/**
 * @brief Get the resource that was requested by the client
 *
 * @param config Configuration to scan through
 * @return const Resource The resource requested
 */
const Resource Request::resource(const std::vector<ServerBlock *> &config) const
{
    if (!_valid)
        return Resource(INVALID_REQUEST);

    // Find server block with the correct hostname
    std::vector<ServerBlock *>::const_iterator matchedServerBlock =
        std::find_if(config.begin(), config.end(), HostNameMatcher(hostname()));

    // If no server block matches the hostname then return a NOT_FOUND resource
    if (matchedServerBlock == config.end())
        return Resource(NO_MATCH, _requestedURL);
    return getResourceFromConfig((*matchedServerBlock)->routes);
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
    _headers.clear();
    _requestedURL.clear();
    _length = 0;
    _httpMethod = OTHER;
    _valid = true;
}

/**
 * @brief Destroy the Request object
 */
Request::~Request()
{
    delete[] _buffer;
}
