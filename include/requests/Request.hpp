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

/**
 * @brief This class defines an HTTP request
 */
class Request
{
  private:
    HTTPMethod _httpMethod;
    std::string _pathToTarget;
    std::map<std::string, std::string> _headers;
    std::string _rawBody;

  public:
    Request(const char *rawReq);
    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    const HTTPMethod &method() const;
    const std::string &target(const ServerBlock &config) const;
    const std::string &body() const;

  private:
    void parseRequest(const std::string &reqStr);
    void extracted(std::stringstream &reqStream, std::string &token);
    void parseStartLine(std::stringstream &reqStream);
    void stripLineEnding(std::stringstream &reqStream);
    std::string stripLineEnding(const std::string &token);
    void validateToken(const std::stringstream &reqStream, const std::string &token,
                       const std::string &errMsg);
    void checkEOF(const std::stringstream &reqStream);
};

void requestParsingTests();

#endif
