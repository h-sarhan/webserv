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
#include "config/Parser.hpp"
#include "requests/InvalidRequestError.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

RequestType strToRequestType(const std::string &str)
{
    if (str == "OK")
        return OK;
    if (str == "REDIRECTION")
        return REDIRECTION;
    if (str == "METHOD_NOT_ALLOWED")
        return METHOD_NOT_ALLOWED;
    if (str == "DIRECTORY")
        return DIRECTORY;
    return NOT_FOUND;
}

std::string requestTypeToStr(RequestType tkn)
{
    switch (tkn)
    {
    case OK:
        return "OK";
    case REDIRECTION:
        return "REDIRECTION";
    case NOT_FOUND:
        return "NOT_FOUND";
    case METHOD_NOT_ALLOWED:
        return "METHOD_NOT_ALLOWED";
    case DIRECTORY:
        return "DIRECTORY";
        break;
    }
}

RequestTarget::RequestTarget(const RequestType &type, const std::string &resource)
    : type(type), resource(resource)
{
}

static void trimWhitespace(std::string &str)
{
    // left trim
    str.erase(0, str.find_first_not_of(WHITESPACE));
    // right trim
    str.erase(str.find_last_not_of(WHITESPACE) + 1);
}

Request::Request()
{
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

const std::string &Request::body() const
{
    return _rawBody;
}

bool Request::parseRequest(const std::string &reqStr)
{
    if (reqStr.empty())
        throw InvalidRequestError("Empty request");
    const size_t bodyStart = reqStr.find("\r\n\r\n");
    std::stringstream reqStream;
    if (bodyStart == std::string::npos)
        return false;
    else
    {
        reqStream.str(reqStr.substr(0, bodyStart + 2));
        // _rawBody = reqStr.substr(bodyStart + 4);
    }

    parseStartLine(reqStream);
    while (reqStream.peek() != '\r' && !reqStream.eof())
    {
        parseHeader(reqStream);
    }
    return true;
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
    checkLineEnding(reqStream);
}

void Request::checkLineEnding(std::stringstream &reqStream)
{
    std::string lineEnding(2, '\0');

    reqStream >> std::noskipws >> lineEnding[0];
    reqStream >> lineEnding[1];
    reqStream >> std::skipws;
    if (lineEnding[0] != '\r' || lineEnding[1] != '\n')
        throw InvalidRequestError("Invalid start line");
}

void Request::parseHeader(std::stringstream &reqStream)
{
    std::string key;
    reqStream >> key;
    if (key.length() < 2)
        throw InvalidRequestError("Invalid header");

    char sep = *--key.end();
    if (sep != ':')
        throw InvalidRequestError("Invalid header");

    std::string value;
    char c = '\0';
    reqStream >> std::noskipws;
    while (reqStream.peek() != '\r' && !reqStream.eof())
    {
        reqStream >> c;
        value.push_back(c);
    }
    if (reqStream.eof())
        throw InvalidRequestError("Invalid header");

    checkLineEnding(reqStream);
    trimWhitespace(value);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    _headers.insert(std::make_pair(key.substr(0, key.length() - 1), value));
}

std::string Request::userAgent()
{
    if (_headers.count("user-agent") != 0)
        return _headers["user-agent"];
    return "unknown";
}

std::string Request::host()
{
    if (_headers.count("host") != 0)
    {
        std::string hostValue = _headers["host"];
        const size_t colonPos = hostValue.find(":");
        if (colonPos != std::string::npos)
            return hostValue.substr(0, colonPos);
        return hostValue;
    }
    return "localhost";
}

bool Request::keepAlive()
{
    if (_headers.count("connection") != 0)
    {
        if (_headers["connection"] == "closed")
            return false;
        return true;
    }
    return true;
}

unsigned int Request::keepAliveTimer()
{
    if (_headers.count("keep-alive") != 0)
    {
        const std::string &keepAliveValue = _headers["keep-alive"];
        const size_t timeoutPos = keepAliveValue.find("timeout");
        if (timeoutPos == std::string::npos || timeoutPos + 8 > keepAliveValue.length())
            return 5;
        std::stringstream timeoutStream(keepAliveValue.substr(timeoutPos + 8));
        unsigned int timeout;
        timeoutStream >> timeout;
        if (timeoutStream.good() && timeout < 20)
            return timeout;
        return 5;
    }
    if (!keepAlive())
        return 0;
    return 5;
}

unsigned int Request::maxReconnections()
{
    if (_headers.count("keep-alive") != 0)
    {
        const std::string &keepAliveValue = _headers["keep-alive"];
        const size_t maxPos = keepAliveValue.find("max");
        if (maxPos == std::string::npos || maxPos + 4 > keepAliveValue.length())
            return 20;
        std::stringstream timeoutStream(keepAliveValue.substr(maxPos + 4));
        unsigned int maxReconnections;
        timeoutStream >> maxReconnections;
        if (timeoutStream.good() && maxReconnections < 200)
            return maxReconnections;
        return 100;
    }
    if (!keepAlive())
        return 1;
    return 100;
}

static bool matchTargetToRoute(std::string target, std::string route)
{
    // /a/b/ == /a/b
    // /a/b/ == /a/b/
    // /a/b == /a/b

    // /a/b/ == /a
    // /a/b/ == /a/
    if (*--target.end() != '/')
        target.append("/");
    if (*--route.end() != '/')
        route.append("/");
    return target.find(route) != std::string::npos;
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

const RequestTarget Request::getTargetFromServerConfig(std::string &match,
                                                       const ServerBlock &serverConfig)
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
        if (matchTargetToRoute(_target, routeStr) && routeStr.length() > match.length())
            match = routeStr;
    }
    if (match.length() == 0)
        return RequestTarget(NOT_FOUND, "");
    const Route &matchedRoute = serverConfig.routes.at(match);
    if (matchedRoute.methodsAllowed.count(_httpMethod) == 0)
        return RequestTarget(METHOD_NOT_ALLOWED, "");
    if (matchedRoute.redirectTo.length() > 0)
        return RequestTarget(REDIRECTION, matchedRoute.redirectTo);
    if (matchedRoute.serveDir.length() > 0)
    {
        std::string requestTarget =
            matchedRoute.serveDir + "/" + _target.substr(_target.find(match));

        removeDoubleSlash(requestTarget);
        // Check if the target exists
        struct stat info;
        if (stat(requestTarget.c_str(), &info) == 0)
        {
            // Check if the target is a file or a directory
            if (info.st_mode & S_IFREG)
                return RequestTarget(OK, requestTarget);
            if (info.st_mode & S_IFDIR)
                return RequestTarget(DIRECTORY, requestTarget);
        }
    }
    return RequestTarget(NOT_FOUND, "");
}

const RequestTarget Request::target(serverList serverBlocks)
{
    std::string biggestMatch;
    for (std::vector<ServerBlock>::const_iterator blockIt = serverBlocks.begin();
         blockIt != serverBlocks.end(); blockIt++)
    {
        if (blockIt->hostname == host())
            return getTargetFromServerConfig(biggestMatch, *blockIt);
    }
    return RequestTarget(NOT_FOUND, "");
}

void Request::setBody(const std::string &body)
{
    _rawBody = body;
}

static bool testRequest(const std::string &rawRequest)
{
    try
    {
        Request request;
        request.parseRequest(rawRequest);
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
    assert(testRequest("GET\r\n\r\n") == false);
    assert(testRequest("GET \r\n\r\n") == false);
    assert(testRequest("GET fsfd\r\n\r\n") == false);
    assert(testRequest("GET fsfd HTTP/2.0\r\n\r\n") == false);
    assert(testRequest("GET  HTTP/1.0\r\n\r\n") == false);
    assert(testRequest("POSTT / HTTP/1.1\r\n\r\n") == false);

    // Start line pass tests
    assert(testRequest("PUT /fds HTTP/1.0\r\n\r\n") == true);
    assert(testRequest("GET fsfd HTTP/1.1\r\n\r\n") == true);
    assert(testRequest("GET fsfd HTTP/1.0\r\n\r\n") == true);
    assert(testRequest("POST / HTTP/1.1\r\n\r\n") == true);
    assert(testRequest("PUT /fds HTTP/1.0\r\n\r\n") == true);
    assert(testRequest("DELETE /fds HTTP/1.0\r\n\r\n") == true);

    // Empty body tests with no headers
    assert(testRequest("PUT /fds HTTP/1.0\r\n\r\n") == true);

    // Normal body test with no headers
    assert(testRequest("PUT /fds HTTP/1.0\r\n\r\nThis is a body") == true);

    // HTTP Method tests
    Request delReq;

    delReq.parseRequest("DELETE /fds HTTP/1.0\r\n\r\n");
    assert(delReq.method() == DELETE);

    Request getReq;

    getReq.parseRequest("GET /fds HTTP/1.0\r\n\r\n");
    assert(getReq.method() == GET);

    Request postReq;

    postReq.parseRequest("POST /fds HTTP/1.0\r\n\r\n");
    assert(postReq.method() == POST);

    Request putReq;

    putReq.parseRequest("PUT /fds HTTP/1.0\r\n\r\n");
    assert(putReq.method() == PUT);

    // Header tests
    assert(testRequest("PUT gerp HTTP/1.0\r\nContent-Length: 1000\r\nTransfer-encoding: "
                       "chunked\r\nUser-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; "
                       "rv:50.0) Gecko/20100101 Firefox/50.0\r\nHost: webserv.com\r\n\r\n") ==
           true);
    assert(testRequest("PUT gerp HTTP/1.0\r\nContent-Length: 1000\r\nTransfer-encoding: "
                       "chunked\r\nUser-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; "
                       "rv:50.0) Gecko/20100101 Firefox/50.0\r\nHost webserv.com\r\n\r\n") ==
           false);

    // Test big GET with a bunch of headers and body should pass
    assert(
        testRequest("GET /favicon.ico HTTP/1.1\r\n"
                    "Host: localhost:1234\r\nConnection: keep-alive\r\n"
                    "sec-ch-ua-mobile: ?0\r\n"
                    "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)\r\n"
                    "Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n"
                    "Referer: http://localhost:1234/\r\n"
                    "Accept-Encoding: gzip, deflate, br\r\n"
                    "Accept-Language: en-US,en;q=0.9,ar-XB;q=0.8,ar;q=0.7\r\n\r\nergrere") == true);

    const char *getReqStr = "GET meme.jpeg HTTP/1.1\r\n"
                            "Host: webserv.com:1234\r\n"
                            "Connection: Keep-Alive\r\n"
                            "Content-Encoding: gzip\r\n"
                            "Content-Type: text/html; charset=utf-8\r\n"
                            "Date: Thu, 11 Aug 2016 15:23:13 GMT\r\n"
                            "Keep-Alive: timeout=5, max=1000\r\n"
                            "Last-Modified: Mon, 25 Jul 2016 04:32:39 GMT\r\n"
                            "Server: Apache\r\n\r\nmeh";

    Request getReqTest;

    getReqTest.parseRequest(getReqStr);

    assert(getReqTest.keepAlive() == true);
    assert(getReqTest.keepAliveTimer() == 5);
    // assert(getReqTest.body() == "meh");
    assert(getReqTest.host() == "webserv.com");
    assert(getReqTest.maxReconnections() == 100);
    assert(getReqTest.method() == GET);
    assert(getReqTest.userAgent() == "unknown");

    // Test big GET with a bunch of headers and body should fail
    assert(
        testRequest(
            "GET /favicon.ico HTTP/1.1\r\nHost: localhost:1234\r\n: "
            "keep-alive\r\nsec-ch-ua: "
            "\"Not_A Brand\";v=\"99\", \"Google Chrome\";v=\"109\", "
            "\"Chromium\";v=\"109\"\r\nsec-ch-ua-mobile: ?0\r\nUser-Agent: Mozilla/5.0 (Macintosh; "
            "Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 "
            "Safari/537.36\r\nsec-ch-ua-platform: \"macOS\"\r\nAccept: "
            "image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\nSec-Fetch-Site: "
            "same-origin\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Dest: image\r\nReferer: "
            "http://localhost:1234/\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: "
            "en-US,en;q=0.9,ar-XB;q=0.8,ar;q=0.7\r\n\r\n") == false);

    const std::string &getCashew1 = "GET /cashew/ HTTP/1.1\r\nHost: webserv\r\n\r\n";

    Request req;
    req.parseRequest(getCashew1);
    Parser parser("target_tests.conf");
    RequestTarget target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("1") != std::string::npos);

    const std::string &getCashew2 = "GET /cashews/milk.html HTTP/1.1\r\nHost: webserv\r\n\r\n";
    req.parseRequest(getCashew2);
    target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("2") != std::string::npos);

    const std::string &getCashew3 =
        "GET /cashews/honey/cake/bottom.html HTTP/1.1\r\nHost: webserv\r\n\r\n";
    req.parseRequest(getCashew3);
    target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("3") != std::string::npos);

    const std::string &getCashew4 = "GET /cashews/juice HTTP/1.1\r\nHost: webserv\r\n\r\n";
    req.parseRequest(getCashew4);
    target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("4") != std::string::npos);

    const std::string &getCashew4Again = "GET /cashews/juice/ HTTP/1.1\r\nHost: webserv\r\n\r\n";
    req.parseRequest(getCashew4Again);
    target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("4") != std::string::npos);

    const std::string &getCashew2Again =
        "GET /cashews/juicekiuhb HTTP/1.1\r\nHost: webserv\r\n\r\n";
    req.parseRequest(getCashew2Again);
    target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("2") != std::string::npos);

    const std::string &getCashew6 = "GET /cashews/honey/cak HTTP/1.1\r\nHost: webserv\r\n\r\n";
    req.parseRequest(getCashew6);
    target = req.target(parser.getConfig());
    assert(target.type == REDIRECTION);
    assert(target.resource.find("6") != std::string::npos);

    const std::string &getAssets = "GET /Bobby.html HTTP/1.1\r\nHost: rew\r\n\r\n";
    Request reqAssets;
    reqAssets.parseRequest(getAssets);
    target = reqAssets.target(parser.getConfig());
    assert(target.type == NOT_FOUND);

    const std::string &getDir = "GET /artgallerycontent HTTP/1.1\r\nHost: rew\r\n\r\n";
    reqAssets.parseRequest(getDir);
    target = reqAssets.target(parser.getConfig());
    assert(target.type == DIRECTORY);
    assert(target.resource == "./assets/artgallerycontent");

    const std::string &deleteAssets = "DELETE /Bobby.html HTTP/1.1\r\nHost: rew\r\n\r\n";
    reqAssets.parseRequest(deleteAssets);
    target = reqAssets.target(parser.getConfig());
    assert(target.type == METHOD_NOT_ALLOWED);

    const std::string &getImg = "GET /artgallerycontent/2020_3.JPG HTTP/1.1\r\nHost: rew\r\n\r\n";
    reqAssets.parseRequest(getImg);
    target = reqAssets.target(parser.getConfig());
    assert(target.type == OK);
    assert(target.resource == "./assets/artgallerycontent/2020_3.JPG");
}

Request::~Request()
{
}
