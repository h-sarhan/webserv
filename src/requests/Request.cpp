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

Request::Request(const char *rawReq)
{
    // * rawReq needs to null terminated
    parseRequest(rawReq);
}

Request::Request(const Request &req)
    : _httpMethod(req.method()), _pathToTarget(req._pathToTarget), _headers(req._headers),
      _rawBody(req._rawBody)
{
}

Request &Request::operator=(const Request &req)
{
    if (&req == this)
        return *this;
    _httpMethod = req._httpMethod;
    _pathToTarget = req._pathToTarget;
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

const std::string &Request::body() const
{
    return _rawBody;
}

void Request::parseRequest(const std::string &reqStr)
{
    if (reqStr.empty())
        throw InvalidRequestError("Empty request");
    std::stringstream reqStream(reqStr);
    parseStartLine(reqStream);
    if (reqStream.eof())
        return;
    // parseHeaders(reqStream);
}

void Request::validateToken(const std::stringstream &reqStream, const std::string &token,
                            const std::string &errMsg)
{
    if (reqStream.bad() || token.empty())
        throw InvalidRequestError(errMsg);
}

void Request::checkEOF(const std::stringstream &reqStream)
{
    if (reqStream.bad() || reqStream.eof())
        throw InvalidRequestError("Invalid request");
}

void Request::parseStartLine(std::stringstream &reqStream)
{
    // * Examples of valid start lines
    // GET /background.png HTTP/1.0
    // POST / HTTP/1.1
    // DELETE /src/main.cpp HTTP/1.1

    std::string token;
    std::getline(reqStream, token, ' ');

    validateToken(reqStream, token, "Invalid start line");
    checkEOF(reqStream);
    if (strToHTTPMethod(token) == OTHER)
        throw InvalidRequestError("Invalid/unsupported HTTP method");

    _httpMethod = strToHTTPMethod(token);

    std::getline(reqStream, token, ' ');

    validateToken(reqStream, token, "Invalid start line");
    checkEOF(reqStream);

    _pathToTarget = token;

    std::getline(reqStream, token, ' ');

    validateToken(reqStream, token, "Invalid start line");

    token = stripLineEnding(token);

    if (token != "HTTP/1.0" && token != "HTTP/1.1")
        throw InvalidRequestError("Invalid/unsupported HTTP version");
}

std::string Request::stripLineEnding(const std::string &token)
{
    const size_t lineEndPos = token.find_first_of("\r\n");
    if (lineEndPos == std::string::npos)
        throw InvalidRequestError("Incorrect line ending in request");

    const std::string lineEnd = token.substr(lineEndPos, token.length());
    if (lineEnd.length() != 2)
        throw InvalidRequestError("Incorrect line ending in request");

    if (lineEnd[0] != '\r')
        throw InvalidRequestError("Incorrect line ending in request");
    if (lineEnd[1] != '\n')
        throw InvalidRequestError("Incorrect line ending in request");

    return token.substr(0, token.length() - 2);
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
    assert(testRequest("") == false);
    assert(testRequest("GET") == false);
    assert(testRequest("GET ") == false);
    assert(testRequest("GET fsfd") == false);
    assert(testRequest("GET fsfd HTTP/2.0\r\n") == false);
    assert(testRequest("GET  HTTP/1.0\r\n") == false);
    assert(testRequest("POSTT / HTTP/1.1\r\n") == false);
    assert(testRequest("PUT /fds HTTP/1.0\r\n\r\n") == false);
    assert(testRequest("PUT /fds HTTP/1.0\r") == false);

    assert(testRequest("GET fsfd HTTP/1.1\r\n") == true);
    assert(testRequest("GET fsfd HTTP/1.0\r\n") == true);
    assert(testRequest("POST / HTTP/1.1\r\n") == true);
    assert(testRequest("PUT /fds HTTP/1.0\r\n") == true);
    assert(testRequest("DELETE /fds HTTP/1.0\r\n") == true);

    Request req("DELETE /fds HTTP/1.0\r\n");
    assert(req.method() == DELETE);
    // assert(req.target() == DELETE);
}

Request::~Request()
{
}
