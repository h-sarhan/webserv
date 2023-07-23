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

// ! Major refactoring needed.
// ! Keep this class clean by having it only be responsible for storing data and maintaining the
// ! buffer. If something needs to be parsed then it will be handed over to a RequestParser object
/**
 * @brief This class is responsible for parsing an HTTP request
 */
class Request
{
  private:
    char *_buffer;                                 // * keep
    size_t _length;                                // * keep
    size_t _capacity;                              // * keep
    HTTPMethod _httpMethod;                        // ? put in reqParser
    std::map<std::string, std::string> _headers;   // ? put in reqParser
    size_t _bodyStart;                             // ? put in reqParser
    std::string _requestedURL;                     // ? put in Resource
    bool _valid;                                   // ? put in resource
    int _listener;                                 // * use this for getting serverBlocks
    // size_t _maxSize;                            // ? put in reqParser
    // Resource _resource                          // ? put in reqParser

  public:
    Request(int listener = -1);
    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    // Parse the request method, resource, and headers
    // Returns false if the request does not include the full headers
    // ! ParseRequest should find the value of keepAlive, keepAliveTimer, maxSize, hostname, and
    //                                                                                    ! resource
    bool parseRequest();   // * keep in request but make this call reqParser

    // Getters for attributes that have been parsed from the HTTP request
    const HTTPMethod &method() const;   // * keep these in request
    const char *buffer() const;         // * keep these in request
    size_t length() const;              // * keep these in request
    size_t bodyStart() const;           // * keep these in request
    size_t maxBodySize() const;         // * keep these in request
    // ! Make this const
    std::map<std::string, std::string> &headers();   // keep these in request

    // ! Cache these getters
    bool keepAlive() const;                // ? cache these in reqParser
    unsigned int keepAliveTimer() const;   // ? cache these in reqParser
    // Returns a resource object associated with the request
    const Resource resource() const;   // ? cache these in reqParser

    // Appends request data to the internal buffer
    void appendToBuffer(const char *data, const size_t n);   // * keep this in request

    // Clears the attributes of this request
    void clear();   // * keep this in request but make it call reqParser

    // Unchunks the request if it is chunked, and updates the request and length
    void unchunk();   // ? idk

  private:
    const std::string hostname() const;   // ? cache this in reqParser

    // Parses the first line of an HTTP request e.g. GET /index.html HTTP/1.1
    void parseStartLine(std::stringstream &reqStream);   // ? put this in reqParser

    // Parses a request header. e.g. Host: webserv.com
    void parseHeader(std::stringstream &reqStream);   // ? put this in reqParser

    // Checks if a line ends with \r\n. Throws an exception otherwise
    void checkLineEnding(std::stringstream &reqStream) const;   // ? put this in reqParser

    // Make this accept the server block config from mehrin
    const std::map<std::string, Route>::const_iterator getRequestedRoute(
        const std::map<std::string, Route> &routes) const;   // ? put this in reqParser

    const Resource getResourceFromConfig(
        const std::map<std::string, Route> &routes) const;   // ? put this in reqParser

    // Resizes the internal buffer
    void resizeBuffer(size_t newCapacity);   // * keep this in request

    // Assert that a condition is true, throw an exception otherwise
    void assertThat(bool condition, const std::string &throwMsg) const;   // ? put this in reqParser
};

#endif
