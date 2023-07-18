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
#define WHITESPACE      " \t\n\r\f\v"

// * Important headers
// Host: webserv.com
// Connection: keep-alive
// Keep-Alive: timeout=5, max=1000

/**
 * @brief This class defines an HTTP request
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
    // ! Add flag to see whether the request was parsed correctly
    // bool _valid;

  public:
    Request();

    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    // Returns false if the request does not contain the full headers
    // Throws InvalidRequestError if the request is detected to be invalid
    bool parseRequest();
    const HTTPMethod &method() const;

    const char *buffer() const;
    size_t requestLength() const;

    void appendToBuffer(const char *data, size_t n);

    std::map<std::string, const std::string> &headers();
    const std::string userAgent() const;
    const std::string hostname() const;
    bool keepAlive() const;
    unsigned int keepAliveTimer() const;
    unsigned int maxReconnections() const;
    const Resource resource(std::vector<ServerBlock *> &config) const;

    void clear();

  private:
    void parseStartLine(std::stringstream &reqStream);
    void parseHeader(std::stringstream &reqStream);
    void checkLineEnding(std::stringstream &reqStream);
    const Resource getResourceFromServerConfig(std::string &biggestMatch,
                                               const ServerBlock &serverConfig) const;
    void checkStream(const std::stringstream &reqStream, const std::string &token,
                     const std::string &errMsg);
    void checkEOF(const std::stringstream &reqStream);
    void resizeBuffer(size_t newCapacity);

    void assertThat(bool condition, const std::string &throwMsg) const;
};

// void requestParsingTests();

void requestBufferTests();

#endif
