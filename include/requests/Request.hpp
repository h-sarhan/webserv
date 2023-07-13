/**
 * @file Request.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This class defines the structure of an HTTP Request
 * @date 2023-07-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "config/ServerBlock.hpp"
#include "enums/HTTPMethods.hpp"
#include <map>

#define WHITESPACE " \t\n\r\f\v"

// * Important headers
// Host: webserv.com
// Connection: keep-alive
// Keep-Alive: timeout=5, max=1000
// ? Content-Length: 1000 // mehrins job i think
// ? Transfer-encoding: chunked // mehrins job i think

/**
 * @brief This class defines an HTTP request
 */
class Request
{
  private:
    HTTPMethod _httpMethod;
    std::string _target;
    std::map<std::string, std::string> _headers;
    const char *_rawBody;
    const std::vector<ServerBlock> &_config;
    // * const std::string _host; // Get from header
    // * const bool _keepAlive; // Get from header
    // * const unsigned int _keepAliveFor; // Get from header
    // * const unsigned int _maxReconnections; // Get from header

  public:
    Request(const char *rawReq, const std::vector<ServerBlock> &config);
    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    const HTTPMethod &method() const;
    const std::string &target(const ServerBlock &config) const;

    const std::string &userAgent() const;
    const char *body() const;

  private:
    void parseRequest(const std::string &reqStr);
    void parseStartLine(std::stringstream &reqStream);
    void parseHeader(std::stringstream &reqStream);
    void checkLineEnding(std::stringstream &reqStream);

    void checkStream(const std::stringstream &reqStream, const std::string &token,
                     const std::string &errMsg);
    void checkEOF(const std::stringstream &reqStream);
};

void requestParsingTests();

#endif
