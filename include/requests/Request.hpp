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

// * Important headers?
// Content-Length: 1000
// Transfer-encoding: chunked
// User-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0
// Host: webserv.com

/**
 * @brief This class defines an HTTP request
 */
class Request
{
  private:
    HTTPMethod _httpMethod;
    std::string _target;
    std::map<std::string, std::string> _headers;
    std::string _userAgent;
    char *_rawBody;

  public:
    Request(const char *rawReq);
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
    void parseHeader(const std::string &header);
    void checkLineEnding(const std::string &line);

    void checkStream(const std::stringstream &reqStream, const std::string &token,
                     const std::string &errMsg);
    void checkEOF(const std::stringstream &reqStream);
};

void requestParsingTests();

#endif
