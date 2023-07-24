/**
 * @file Parser.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines the RequestParser class
 * @date 2023-07-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "enums/HTTPMethods.hpp"
#include "requests/Resource.hpp"
#include <map>
#include <string>

/**
 * @brief This class is responsible for parsing the request buffer
 */
class RequestParser
{
  private:
    HTTPMethod _httpMethod;
    std::pair<bool, unsigned int> _keepAlive;
    std::map<std::string, std::string> _headers;
    std::string _hostname;
    size_t _bodyStart;
    size_t _maxSize;
    std::string _requestedURL;
    bool _valid;
    Resource _resource;

  public:
    RequestParser();
    RequestParser(const RequestParser &reqParser);
    RequestParser &operator=(const RequestParser &reqParser);

    // Returns true if the headers have been fully received
    // bool parse(const char *buffer, const std::vector<ServerBlock *> &config);
    bool parse(const char *buffer, size_t len, const std::vector<ServerBlock *> &config);

    // HTTP Request Getters
    const HTTPMethod &method() const;
    const std::pair<bool, unsigned int> &keepAlive() const;
    std::map<std::string, std::string> &headers();   // ! make this const
    size_t bodyStart() const;
    size_t maxBodySize() const;
    const Resource &resource() const;
    const std::string hostname() const;

    // Clears the parser attributes
    void clear();

  private:
    // Parses the first line of an HTTP request e.g. GET /index.html HTTP/1.1
    void parseStartLine(std::stringstream &reqStream);

    // Parses a request header. e.g. Host: webserv.com
    void parseHeader(std::stringstream &reqStream);

    // Checks if a line ends with \r\n. Throws an exception otherwise
    void checkLineEnding(std::stringstream &reqStream) const;

    // Make this accept the server block config from mehrin
    const std::map<std::string, Route>::const_iterator getRequestedRoute(
        const std::map<std::string, Route> &routes) const;

    Resource generateResource(const std::vector<ServerBlock *> &config) const;

    std::string parseHostname() const;
    std::pair<bool, unsigned int> parseKeepAlive() const;
    unsigned int parseBodySize(const std::vector<ServerBlock *> &config) const;

    // Assert that a condition is true, throw an exception otherwise
    void assertThat(bool condition, const std::string &throwMsg) const;
};

#endif
