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

// ! Remove magic numbers

Request::Request()
    : _httpMethod(), _resourcePath(), _headers(), _buffer(new char[REQ_BUFFER_SIZE]), _length(0),
      _capacity(REQ_BUFFER_SIZE), _valid(true)
{
}

Request::Request(const Request &req)
    : _httpMethod(req.method()), _resourcePath(req._resourcePath), _headers(req._headers),
      _buffer(new char[req._capacity]), _length(req._length), _capacity(req._capacity),
      _valid(req._valid)
{
    // Deep copy
    std::memcpy(_buffer, req._buffer, _length);
}

Request &Request::operator=(const Request &req)
{
    if (&req == this)
        return *this;
    _httpMethod = req._httpMethod;
    _resourcePath = req._resourcePath;
    _headers = req._headers;
    _length = req._length;
    _capacity = req._capacity;
    _valid = req._valid;
    delete[] _buffer;
    _buffer = new char[_capacity];
    std::memcpy(_buffer, req._buffer, _length);
    return *this;
}

const HTTPMethod &Request::method() const
{
    return _httpMethod;
}

const char *Request::buffer() const
{
    return _buffer;
}

size_t Request::requestLength() const
{
    return _length;
}

void Request::assertThat(bool condition, const std::string &throwMsg) const
{
    if (!condition)
        throw InvalidRequestError(throwMsg);
}

bool Request::parseRequest()
{
    if (_length == 0)
        return false;
    if (!_headers.empty())
        return true;

    // Search for double CRLF in the request buffer
    const char doubleCRLF[] = "\r\n\r\n";
    char *bodyStart = std::search(_buffer, _buffer + _length, doubleCRLF,
                                  doubleCRLF + sizeOfArray(doubleCRLF) - 1);
    if (bodyStart == _buffer + _length)
        return false;

    // If double CRLF is found then we form a string starting from the beginning of the request till
    // the last header
    const std::string requestHead(_buffer, bodyStart + 2);

    try
    {
        // Create stringstream containing the start line and headers
        std::stringstream reqStream(requestHead);
        parseStartLine(reqStream);

        // Parse headers
        while (reqStream.peek() != '\r' && !reqStream.eof())
            parseHeader(reqStream);
    }
    catch (const InvalidRequestError &e)
    {
        // ! Log here using the logger class that we have recieved an invalid request
        std::cerr << e.what() << std::endl;
        _valid = false;
    }
    return true;
}

void Request::parseStartLine(std::stringstream &reqStream)
{
    // * Examples of valid start lines
    // GET /background.png HTTP/1.0
    // POST / HTTP/1.1
    // DELETE /src/main.cpp HTTP/1.1

    // Parse HTTP method as a string
    const std::string &httpMethod = getNext<std::string>(reqStream);
    assertThat(!httpMethod.empty(), "Invalid start line");

    // Convert HTTP method to an enum
    _httpMethod = strToEnum<HTTPMethod>(httpMethod);
    assertThat(_httpMethod != OTHER, "Invalid start line");

    // Parse the requested resource path
    _resourcePath = getNext<std::string>(reqStream);
    assertThat(!_resourcePath.empty(), "Invalid start line");

    // Clean the URL
    sanitizeURL(_resourcePath);

    // Check HTTP version
    const std::string &httpVersion = getNext<std::string>(reqStream);
    assertThat(httpVersion == "HTTP/1.0" || httpVersion == "HTTP/1.1", "Invalid start line");

    checkLineEnding(reqStream);
}

void Request::checkLineEnding(std::stringstream &reqStream)
{
    std::string lineEnding(2, '\0');

    // Check that the line ends with CRLF
    assertThat(!reqStream.eof(), "Invalid header");
    reqStream >> std::noskipws >> lineEnding[0] >> lineEnding[1] >> std::skipws;
    assertThat(lineEnding == "\r\n", "Invalid start line");
}

void Request::parseHeader(std::stringstream &reqStream)
{
    // Get the key part of the header
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
        value += getNext<char>(reqStream);
    assertThat(!value.empty() && !reqStream.eof(), "Invalid header");

    // Trim value of whitespace and transform to lowercase
    trimStr(value, WHITESPACE);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    checkLineEnding(reqStream);

    // Insert header into map
    _headers.insert(std::make_pair(key, value));
}

const std::string Request::userAgent() const
{
    return _headers.count("user-agent") == 0 ? "unknown" : _headers.at("user-agent");
}

const std::string Request::hostname() const
{
    if (_headers.count("host") == 0)
        return "localhost";

    std::string hostValue = _headers.at("host");
    hostValue = hostValue.substr(0, hostValue.find(":"));

    // Check if the host value is a port rather than a hostname
    const size_t numDigits = std::count_if(hostValue.begin(), hostValue.end(), ::isdigit);
    if (numDigits == hostValue.length())
        return "localhost";

    if (validateHostName(hostValue))
    {
        // ! Log here that an invalid hostname is in the header
        return hostValue;
    }
    return "localhost";
}

std::map<std::string, const std::string> &Request::headers()
{
    return _headers;
}

bool Request::keepAlive() const
{
    return _headers.count("connection") == 0 || _headers.at("connection") != "closed";
}

unsigned int Request::keepAliveTimer() const
{
    if (!keepAlive())
        return 0;

    if (_headers.count("keep-alive") == 0)
        return 5;

    const std::string &keepAliveValue = _headers.at("keep-alive");
    if (keepAliveValue.find("timeout=") == std::string::npos)
        return 5;

    std::stringstream timeOutStream(keepAliveValue.substr(keepAliveValue.find("timeout=")));
    getNext<std::string>(timeOutStream);
    unsigned int timeout = getNext<unsigned int>(timeOutStream);
    if (timeOutStream.good() && timeout < 20)
        return timeout;
    return 5;
}

// We might not use this function
unsigned int Request::maxReconnections() const
{
    if (!keepAlive())
        return 1;

    if (_headers.count("keep-alive") == 0)
        return 20;
    const std::string &keepAliveValue = _headers.at("keep-alive");

    std::stringstream maxConnStream(keepAliveValue.substr(keepAliveValue.find("max=")));
    getNext<std::string>(maxConnStream);
    const unsigned int maxConnections = getNext<unsigned int>(maxConnStream);
    if (maxConnStream.good() && maxConnections < 20)
        return maxConnections;
    return 20;
}

static bool matchResourceToRoute(std::string resource, std::string route)
{
    return resource.find(route) != std::string::npos;
}

static void removeDoubleSlash(std::string &str)
{
    size_t slashPos = str.find("//");
    while (slashPos != std::string::npos)
    {
        str.erase(slashPos, 1);
        slashPos = str.find("//");
    }
}

// ! Refactor this
const Resource Request::getResourceFromServerConfig(std::string &match,
                                                    const ServerBlock &serverConfig) const
{
    for (std::map<std::string, Route>::const_iterator routeIt = serverConfig.routes.begin();
         routeIt != serverConfig.routes.end(); routeIt++)
    {
        std::string routeStr = routeIt->first;
        if (routeStr == "/")
        {
            match = routeStr;
            continue;
        }
        if (matchResourceToRoute(_resourcePath, routeStr) && routeStr.length() > match.length())
            match = routeStr;
    }
    if (match.length() == 0)
        return Resource(NOT_FOUND, "");
    const Route &matchedRoute = serverConfig.routes.at(match);
    if (matchedRoute.methodsAllowed.count(_httpMethod) == 0)
        return Resource(FORBIDDEN_METHOD, "");
    if (matchedRoute.redirectTo.length() > 0)
        return Resource(REDIRECTION, matchedRoute.redirectTo);
    if (matchedRoute.serveDir.length() > 0)
    {
        std::string resourcePath = matchedRoute.serveDir + "/" +
                                   _resourcePath.substr(_resourcePath.find(match) + match.length());

        removeDoubleSlash(resourcePath);
        // Check if the resource exists
        struct stat info;
        if (stat(resourcePath.c_str(), &info) == 0)
        {
            // Check if the resource is a file or a directory
            if (info.st_mode & S_IFREG)
                return Resource(EXISTING_FILE, resourcePath);
            if (info.st_mode & S_IFDIR)
            {
                if (matchedRoute.listDirectories == true)
                    return Resource(DIRECTORY, resourcePath);
                if (matchedRoute.listDirectoriesFile.empty())
                    return Resource(NOT_FOUND, "");
                return Resource(EXISTING_FILE, matchedRoute.listDirectoriesFile);
            }
        }
    }
    return Resource(NOT_FOUND, "");
}

const Resource Request::resource(std::vector<ServerBlock *> &serverBlocks) const
{
    if (!_valid)
        return Resource(INVALID_REQUEST);
    std::string biggestMatch;
    for (std::vector<ServerBlock *>::iterator blockIt = serverBlocks.begin();
         blockIt != serverBlocks.end(); blockIt++)
    {
        if ((*blockIt)->hostname == hostname())
            return getResourceFromServerConfig(biggestMatch, **blockIt);
    }
    return Resource(NOT_FOUND, "");
}

void Request::resizeBuffer(size_t newCapacity)
{
    char *newBuffer = new char[newCapacity];
    std::memcpy(newBuffer, _buffer, _length);
    delete[] _buffer;
    _buffer = newBuffer;
    _capacity = newCapacity;
}

void Request::appendToBuffer(const char *data, size_t n)
{
    if (_length + n >= _capacity)
        resizeBuffer(std::max(_capacity * 2, (_length + n) * 2));
    std::memcpy(_buffer + _length, data, n);
    _length += n;
}

void Request::clear()
{
    _length = 0;
    _headers.clear();
    _resourcePath = "";
}

Request::~Request()
{
    delete[] _buffer;
}
