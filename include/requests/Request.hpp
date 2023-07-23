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
#include "requests/RequestParser.hpp"
#include "requests/Resource.hpp"
#include <map>

#define REQ_BUFFER_SIZE 2000

/**
 * @brief This class is responsible for parsing an HTTP request
 */
class Request
{
  private:
    char *_buffer;
    size_t _length;
    size_t _capacity;
    int _port;
    RequestParser _parser;

  public:
    Request(int listener = -1);
    Request(const Request &req);
    Request &operator=(const Request &req);
    ~Request();

    // Returns false if the request does not include the full headers
    bool parseRequest();

    // Getters for request attributes
    const HTTPMethod &method() const;
    const char *buffer() const;
    size_t length() const;
    size_t bodyStart() const;
    size_t maxBodySize() const;
    std::map<std::string, std::string> &headers();   // ! Make this const
    bool keepAlive() const;
    unsigned int keepAliveTimer() const;

    // Returns a resource object associated with the request
    const Resource &resource() const;

    // Appends request data to the internal buffer
    void appendToBuffer(const char *data, const size_t n);

    // Clears the attributes of this request
    void clear();

    // Unchunks the request if it is chunked, and updates the request and length
    void unchunk();   // ? idk

  private:
    // Resizes the internal buffer
    void resizeBuffer(size_t newCapacity);
};

#endif
