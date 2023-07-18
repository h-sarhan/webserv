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
#include "requests/Resource.hpp"
#include <map>

#define REQ_BUFFER_SIZE 2000

/**
 * @brief This class is responsible for parsing an HTTP request
 */
class Request
{
  private:
    HTTPMethod _httpMethod;
    std::string _resourcePath;
    std::map<std::string, const std::string> _headers;
    char *_buffer;
    size_t _length;
    size_t _capacity;
    bool _valid;

  public:
    Request();
    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    // Parse the request method, resource, and headers
    // Returns true when the request has been parsed
    // Returns false if the request does not include the full headers
    bool parseRequest();

    // Getters for attributes that have been parsed from the HTTP request
    const HTTPMethod &method() const;
    const char *buffer() const;
    size_t requestLength() const;
    std::map<std::string, const std::string> &headers();
    const std::string hostname() const;      // might end up being private
    bool keepAlive() const;                  // might not need these
    unsigned int keepAliveTimer() const;     // might not need these
    unsigned int maxReconnections() const;   // might not need these
    // ! Implement this method
    // ! size_t bodySize() const;

    // Appends request data to the internal buffer
    void appendToBuffer(const char *data, size_t n);

    // Returns a resource object associated with the request
    const Resource resource(std::vector<ServerBlock *> &config) const;

    // Clears the attributes of this request
    void clear();

  private:
    // Parses the first line of an HTTP request e.g. GET /index.html HTTP/1.1
    void parseStartLine(std::stringstream &reqStream);

    // Parses a request header. e.g. Host: webserv.com
    void parseHeader(std::stringstream &reqStream);

    // Checks if a line ends with \r\n. Throws an exception otherwise
    void checkLineEnding(std::stringstream &reqStream);

    // ! To be refactored
    const Resource getResourceFromServerConfig(std::string &biggestMatch,
                                               const ServerBlock &serverConfig) const;

    // Resizes the internal buffer
    void resizeBuffer(size_t newCapacity);

    // Assert that a condition is true, throw an exception otherwise
    void assertThat(bool condition, const std::string &throwMsg) const;
};

// void requestParsingTests();

void requestBufferTests();

#endif
