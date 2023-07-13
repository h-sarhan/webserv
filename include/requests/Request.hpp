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
    std::string _rawBody;
    const std::vector<ServerBlock> &_config;
    const unsigned int _port;

  public:
    Request(const std::string rawReq, const std::vector<ServerBlock> &config, unsigned int port);
    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    const HTTPMethod &method() const;
    const std::string &body() const;
    // std::string target() const;

    // ! Make these const when im not tired
    std::string userAgent();
    std::string host();
    bool keepAlive();
    unsigned int keepAliveTimer();
    unsigned int maxReconnections();

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