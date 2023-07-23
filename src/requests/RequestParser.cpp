/**
 * @file Parser.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the RequestParser class
 * @date 2023-07-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "requests/RequestParser.hpp"
#include "config/Validators.hpp"
#include "enums/conversions.hpp"
#include "requests/InvalidRequestError.hpp"
#include "utils.hpp"

#define DEFAULT_HOSTNAME        "localhost"
#define DEFAULT_KEEP_ALIVE_TIME 5
#define MAX_KEEP_ALIVE_TIME     20
#define MAX_RECONNECTIONS       20
#define DEFAULT_RECONNECTIONS   20

RequestParser::RequestParser()
    : _httpMethod(OTHER), _keepAlive(), _headers(), _hostname(), _bodyStart(0), _maxSize(0),
      _requestedURL(), _valid(true), _resource()
{
}

RequestParser::RequestParser(const RequestParser &reqParser)
    : _httpMethod(reqParser._httpMethod), _keepAlive(reqParser._keepAlive),
      _headers(reqParser._headers), _hostname(reqParser._hostname),
      _bodyStart(reqParser._bodyStart), _maxSize(reqParser._maxSize),
      _requestedURL(reqParser._requestedURL), _valid(reqParser._valid),
      _resource(reqParser._resource)
{
}

// returns true if the headers have been fully received
bool RequestParser::parse(const char *buffer, size_t len, const std::vector<ServerBlock *> &config)
{
    if (!_headers.empty())
        return true;

    // Search for double CRLF in the request buffer, return false if not recieved
    const char doubleCRLF[] = "\r\n\r\n";
    const char *bodyStart =
        std::search(buffer, buffer + len, doubleCRLF, doubleCRLF + sizeOfArray(doubleCRLF) - 1);
    if (bodyStart == buffer + len)
        return false;
    _bodyStart = bodyStart - buffer + 4;

    // If double CRLF is found then we form a stringstream starting from the beginning of the
    // request till the last header
    const std::string requestHead(buffer, bodyStart + 2);
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
        _valid = false;
        return true;
    }
    _hostname = parseHostname();
    _keepAlive = parseKeepAlive();
    _maxSize = parseBodySize(config);
    _resource = generateResource(config);
    return true;
}

std::pair<bool, unsigned int> RequestParser::parseKeepAlive() const
{
    if (_headers.count("connection") != 0 && _headers.at("connection") == "close")
        return std::make_pair(false, 0);

    if (_headers.count("keep-alive") == 0)
        return std::make_pair(true, DEFAULT_KEEP_ALIVE_TIME);

    const std::string &keepAliveValue = _headers.at("keep-alive");
    if (keepAliveValue.find("timeout=") == std::string::npos)
        return std::make_pair(true, DEFAULT_KEEP_ALIVE_TIME);

    std::stringstream timeOutStream(keepAliveValue.substr(keepAliveValue.find("timeout=")));
    const std::string &timeoutStr = getNext<std::string>(timeOutStream);
    if (timeoutStr != "timeout=")
        return std::make_pair(true, DEFAULT_KEEP_ALIVE_TIME);

    unsigned int timeout = getNext<unsigned int>(timeOutStream);
    if (timeOutStream.good() && timeout <= MAX_KEEP_ALIVE_TIME)
        return std::make_pair(true, timeout);
    return std::make_pair(true, DEFAULT_KEEP_ALIVE_TIME);
}

std::string RequestParser::parseHostname() const
{
    if (_headers.count("host") == 0)
        return DEFAULT_HOSTNAME;

    std::string hostValue = _headers.at("host");
    hostValue = hostValue.substr(0, hostValue.find(":"));

    // Check if the host value is a port rather than a hostname
    const size_t numDigits = std::count_if(hostValue.begin(), hostValue.end(), ::isdigit);
    if (numDigits == hostValue.length())
        return DEFAULT_HOSTNAME;

    if (validateHostName(hostValue) == false)
    {
        // ! Log here that an invalid hostname is in the header
        return DEFAULT_HOSTNAME;
    }
    return hostValue;
}

unsigned int RequestParser::parseBodySize(const std::vector<ServerBlock *> &config) const
{
    // Find server block with the correct hostname
    std::vector<ServerBlock *>::const_iterator matchedServerBlock =
        std::find_if(config.begin(), config.end(), HostNameMatcher(hostname()));

    if (matchedServerBlock == config.end())
        return std::numeric_limits<unsigned int>::max();
    const std::map<std::string, Route> &routes = (*matchedServerBlock)->routes;
    const std::map<std::string, Route>::const_iterator &routeIt = getRequestedRoute(routes);
    if (routeIt == routes.end())
        return std::numeric_limits<unsigned int>::max();

    const Route &routeOptions = routeIt->second;
    return routeOptions.bodySize;
}

// HTTP Request Getters
const HTTPMethod &RequestParser::method() const
{
    return _httpMethod;
}

const std::pair<bool, unsigned int> &RequestParser::keepAlive() const
{
    return _keepAlive;
}

std::map<std::string, std::string> &RequestParser::headers()
{
    return _headers;
}

size_t RequestParser::bodyStart() const
{
    return _bodyStart;
}

size_t RequestParser::maxBodySize() const
{
    return _maxSize;
}

const Resource &RequestParser::resource() const
{
    return _resource;
}

const std::string RequestParser::hostname() const
{
    return _hostname;
}

// Clears the parser attributes
void RequestParser::clear()
{
    _headers.clear();
    _resource.originalRequest.clear();
    _resource.path.clear();
    _resource.type = NO_MATCH;
    _requestedURL.clear();
    _httpMethod = OTHER;
    _keepAlive.first = true;
    _keepAlive.second = DEFAULT_KEEP_ALIVE_TIME;
    _hostname.clear();
    _bodyStart = 0;
    _maxSize = 0;
}

/**
 * @brief Check that a condition is true, else throw an InvalidRequestError exception
 *
 * @param condition Condition to check
 * @param throwMsg Error message
 */
void RequestParser::assertThat(bool condition, const std::string &throwMsg) const
{
    if (!condition)
        throw InvalidRequestError(throwMsg);
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
void RequestParser::parseStartLine(std::stringstream &reqStream)
{
    // Parse HTTP method as a string
    const std::string &httpMethod = getNext<std::string>(reqStream);
    assertThat(!httpMethod.empty(), "Invalid HTTP method in start line");

    // Convert HTTP method to an enum
    _httpMethod = strToEnum<HTTPMethod>(httpMethod);
    assertThat(_httpMethod != OTHER, "Invalid HTTP method in start line");

    // Parse the requested resource path
    const std::string &requestURL = getNext<std::string>(reqStream);
    assertThat(!requestURL.empty(), "Invalid resource in start line");

    // Clean the resource URL
    _requestedURL = sanitizeURL(requestURL);

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
void RequestParser::checkLineEnding(std::stringstream &reqStream) const
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
void RequestParser::parseHeader(std::stringstream &reqStream)
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

// ! This can be refactored into a function object that is passed to std::find()
const std::map<std::string, Route>::const_iterator RequestParser::getRequestedRoute(
    const std::map<std::string, Route> &routes) const
{
    // const std::string &requestedURL = _resource.originalRequest;
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

Resource RequestParser::generateResource(const std::vector<ServerBlock *> &config) const
{
    if (!_valid)
        return Resource(INVALID_REQUEST, "");

    // Find server block with the correct hostname
    const std::vector<ServerBlock *>::const_iterator matchedServerBlock =
        std::find_if(config.begin(), config.end(), HostNameMatcher(hostname()));

    // If no server block matches the hostname then return a NOT_FOUND resource
    if (matchedServerBlock == config.end())
        return Resource(NO_MATCH, _requestedURL);

    const std::map<std::string, Route> &routes = (*matchedServerBlock)->routes;
    const std::map<std::string, Route>::const_iterator &routeIt = getRequestedRoute(routes);

    if (routeIt == routes.end())
        return Resource(NO_MATCH, _requestedURL);

    const Route &routeOptions = routeIt->second;
    if (routeOptions.methodsAllowed.count(_httpMethod) == 0)
        return Resource(FORBIDDEN_METHOD, _requestedURL, "", routeOptions);

    if (routeOptions.redirectTo.length() > 0)
    {
        std::string resourcePath(_requestedURL);
        resourcePath.erase(0, routeIt->first.length());
        resourcePath.insert(resourcePath.begin(), routeOptions.redirectTo.begin(),
                            routeOptions.redirectTo.end());
        resourcePath = sanitizeURL(resourcePath);
        return Resource(REDIRECTION, _requestedURL, resourcePath, routeOptions);
    }

    const std::string &resourcePath =
        sanitizeURL(routeOptions.serveDir + "/" + _requestedURL.substr(routeIt->first.length()));

    if (!exists(resourcePath))
        return Resource(NOT_FOUND, _requestedURL, resourcePath, routeOptions);

    if (isFile(resourcePath))
        return Resource(EXISTING_FILE, _requestedURL, resourcePath, routeOptions);

    if (_httpMethod == GET || _httpMethod == HEAD)
    {
        const std::string &indexFile = sanitizeURL(resourcePath + "/" + routeOptions.indexFile);
        if (isDir(resourcePath) && isFile(indexFile))
            return Resource(EXISTING_FILE, _requestedURL, indexFile, routeOptions);

        if (isDir(resourcePath) && routeOptions.autoIndex == true)
            return Resource(DIRECTORY, _requestedURL, resourcePath, routeOptions);

        if (isDir(resourcePath) && !isFile(indexFile))
            return Resource(NOT_FOUND, _requestedURL, indexFile, routeOptions);
        return Resource(NOT_FOUND, _requestedURL, resourcePath, routeOptions);
    }
    return Resource(DIRECTORY, _requestedURL, resourcePath, routeOptions);
}