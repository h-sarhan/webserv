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
#include "requests/InvalidRequestError.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

// static void trimWhitespace(std::string &str)
// {
//     // left trim
//     str.erase(0, str.find_first_not_of(WHITESPACE));
//     // right trim
//     str.erase(str.find_last_not_of(WHITESPACE) + 1);
// }

Request::Request(const char *rawReq) : _rawBody(NULL)
{
    // * rawReq needs to null terminated
    parseRequest(rawReq);
}

Request::Request(const Request &req)
    : _httpMethod(req.method()), _target(req._target), _headers(req._headers),
      _rawBody(req._rawBody)
{
}

Request &Request::operator=(const Request &req)
{
    if (&req == this)
        return *this;
    _httpMethod = req._httpMethod;
    _target = req._target;
    _headers = req._headers;
    _rawBody = req._rawBody;
    return *this;
}

const HTTPMethod &Request::method() const
{
    return _httpMethod;
}

// ! Also check if the request method is valid for the server block
// const std::string &Request::target(const ServerBlock &config) const
// {
//     return _pathToTarget;
// }

const char *Request::body() const
{
    return _rawBody;
}

void Request::parseRequest(const std::string &reqStr)
{
    std::stringstream reqStream(reqStr);

    parseStartLine(reqStream);
}

void Request::parseStartLine(std::stringstream &reqStream)
{
    // * Examples of valid start lines
    // GET /background.png HTTP/1.0
    // POST / HTTP/1.1
    // DELETE /src/main.cpp HTTP/1.1
    std::string httpMethod;
    reqStream >> httpMethod;
    if (httpMethod.empty())
        throw InvalidRequestError("Invalid start line");
    _httpMethod = strToHTTPMethod(httpMethod);

    if (_httpMethod == OTHER)
        throw InvalidRequestError("Invalid start line");

    std::string requestTarget;
    reqStream >> requestTarget;
    if (requestTarget.empty())
        throw InvalidRequestError("Invalid start line");
    _target = requestTarget;

    std::string httpVersion;
    reqStream >> httpVersion;
    if (httpVersion.empty() || httpVersion.length() <= 2)
        throw InvalidRequestError("Invalid start line");

    if (httpVersion != "HTTP/1.0" && httpVersion != "HTTP/1.1")
        throw InvalidRequestError("Invalid start line");
    std::string lineEnding(2, '\0');

    reqStream >> std::noskipws >> lineEnding[0];
    reqStream >> lineEnding[1];
    reqStream >> std::skipws;
    if (lineEnding[0] != '\r' || lineEnding[1] != '\n')
        throw InvalidRequestError("Invalid start line");
}

void Request::parseHeader(const std::string &header)
{
    (void) header;
}

static bool testRequest(const char *req)
{
    try
    {
        Request request(req);
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

void requestParsingTests()
{
    (void) testRequest;

    // Start line fail tests
    assert(testRequest("") == false);
    assert(testRequest("GET") == false);
    assert(testRequest("GET ") == false);
    assert(testRequest("GET fsfd") == false);
    assert(testRequest("GET fsfd HTTP/2.0\r\n") == false);
    assert(testRequest("GET  HTTP/1.0\r\n") == false);
    assert(testRequest("POSTT / HTTP/1.1\r\n") == false);
    assert(testRequest("PUT /fds HTTP/1.0\r") == false);

    // Start line pass tests
    assert(testRequest("GET fsfd HTTP/1.1\r\n") == true);
    assert(testRequest("GET fsfd HTTP/1.0\r\n") == true);
    assert(testRequest("POST / HTTP/1.1\r\n") == true);
    assert(testRequest("PUT /fds HTTP/1.0\r\n") == true);
    assert(testRequest("DELETE /fds HTTP/1.0\r\n") == true);

    // Empty body tests
    // assert(testRequest("PUT /fds HTTP/1.0\r\n\r\n") == true);

    // HTTP Method tests
    Request delReq("DELETE /fds HTTP/1.0\r\n");
    assert(delReq.method() == DELETE);
    Request getReq("GET /fds HTTP/1.0\r\n");
    assert(getReq.method() == GET);
    Request postReq("POST /fds HTTP/1.0\r\n");
    assert(postReq.method() == POST);
    Request putReq("PUT /fds HTTP/1.0\r\n");
    assert(putReq.method() == PUT);

    // Header tests
    // assert(testRequest("PUT gerp HTTP/1.0\r\nContent-Length: 1000\r\nTransfer-encoding: "
    //                    "chunked\r\nUser-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; "
    //                    "rv:50.0) Gecko/20100101 Firefox/50.0\r\nHost: webserv.com\r\n") == true);
    // assert(testRequest("PUT gerp HTTP/1.0\r\nContent-Length: 1000\r\nTransfer-encoding: "
    //                    "chunked\r\nUser-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; "
    //                    "rv:50.0) Gecko/20100101 Firefox/50.0\r\nHost webserv.com\r\n") == false);
}

Request::~Request()
{
}
