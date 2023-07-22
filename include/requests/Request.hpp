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

#define DEFAULT_HOSTNAME        "localhost"
#define REQ_BUFFER_SIZE         2000
#define DEFAULT_KEEP_ALIVE_TIME 5
#define MAX_KEEP_ALIVE_TIME     20
#define MAX_RECONNECTIONS       20
#define DEFAULT_RECONNECTIONS   20

/**
 * @brief This class is responsible for parsing an HTTP request
 */
class Request
{
  private:
    HTTPMethod _httpMethod;
    std::string _requestedURL;
    std::map<std::string, const std::string> _headers;
    char *_buffer;
    size_t _length;
    size_t _capacity;
    size_t _bodyStart;
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
    size_t bodyStart() const;

    // ! Make this const
    std::map<std::string, const std::string> &headers();

    // !! Cache these getters
    const std::string hostname() const;      // might end up being private
    bool keepAlive() const;                  // might not need this
    unsigned int keepAliveTimer() const;     // might not need this
    unsigned int maxReconnections() const;   // might not need this
    size_t bodySize() const;
    // std::string rawTarget();
    // Appends request data to the internal buffer
    void appendToBuffer(const char *data, const size_t n);

    // !! Cache this
    // Returns a resource object associated with the request
    const Resource resource(const std::vector<ServerBlock *> &config) const;

    // Clears the attributes of this request
    void clear();

  private:
    // Parses the first line of an HTTP request e.g. GET /index.html HTTP/1.1
    void parseStartLine(std::stringstream &reqStream);

    // Parses a request header. e.g. Host: webserv.com
    void parseHeader(std::stringstream &reqStream);

    // Checks if a line ends with \r\n. Throws an exception otherwise
    void checkLineEnding(std::stringstream &reqStream) const;

    const std::map<std::string, Route>::const_iterator getMatchingRoute(
        const std::map<std::string, Route> &routes) const;

    const Resource getResourceFromConfig(const std::map<std::string, Route> &routes) const;

    // Resizes the internal buffer
    void resizeBuffer(size_t newCapacity);

    // Assert that a condition is true, throw an exception otherwise
    void assertThat(bool condition, const std::string &throwMsg) const;
};

#endif
