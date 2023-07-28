/**
 * @file Response.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of Response class
 * @date 2023-07-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "responses/Response.hpp"
#include "logger/Logger.hpp"
#include "responses/DefaultPages.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstring>
#include <sys/poll.h>
// #include <sys/syslimits.h>
#include <unistd.h>
#include "network/SystemCallException.hpp"

#define WRITE_MAX 65535
#define READ_MAX 1024
#define WRITE_SIZE(x) (x <= WRITE_MAX ? x : WRITE_MAX)

Response::Response() : _buffer(NULL), _length(0), _totalBytesSent(0), _statusCode(0)
{
}

Response::Response(const Response &r)
    : _buffer(NULL), _length(r._length), _totalBytesSent(r._totalBytesSent),
      _statusCode(r._statusCode)
{
    if (this->_buffer != NULL)
        delete[] _buffer;
    this->_buffer = new char[r._length];
    for (size_t i = 0; i < r._length; i++)
        this->_buffer[i] = r._buffer[i];
}

Response &Response::operator=(const Response &r)
{
    if (this != &r)
    {
        if (this->_buffer != NULL)
            delete[] _buffer;
        this->_buffer = new char[r._length];
        for (size_t i = 0; i < r._length; i++)
            this->_buffer[i] = r._buffer[i];
        this->_length = r._length;
        this->_totalBytesSent = r._totalBytesSent;
        this->_statusCode = r._statusCode;
    }
    return (*this);
}

char *Response::buffer()
{
    return _buffer;
}

size_t Response::length()
{
    return _length;
}

size_t Response::totalBytesSent()
{
    return _totalBytesSent;
}

int Response::statusCode()
{
    return _statusCode;
}

void Response::clear()
{
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = NULL;
    _length = 0;
    _totalBytesSent = 0;
    _statusCode = 0;
}

int Response::sendResponse(int fd)
{
    ssize_t bytesSent;

    if (_length == 0)
    {
        // std::cout << "Connection is idle: " << fd << std::endl;
        return IDLE_CONNECTION;
    }
    Log(INFO) << "Sending a response... " << std::endl;
    bytesSent = send(fd, _buffer + _totalBytesSent, _length - _totalBytesSent, 0);
    if (bytesSent < 0)
    {
        Log(ERR) << "Sending response failed: " << strerror(errno) << std::endl;
        return SEND_FAIL;
    }
    else
    {
        _totalBytesSent += bytesSent;
        if (_totalBytesSent < _length)   // partial send
        {
            Log(WARN) << "Response only sent partially: " << bytesSent
                      << ". Total: " << _totalBytesSent << std::endl;
            return SEND_PARTIAL;
        }
    }
    Log(SUCCESS) << "Response sent to connection " << fd << ". Size = " << _totalBytesSent
                 << std::endl;
    return SEND_SUCCESS;
}

template <typename streamType> static size_t getStreamLen(streamType &s)
{
    ssize_t len;

    s.seekg(0, std::ios::end);
    len = s.tellg();
    s.seekg(0, std::ios::beg);
    if (len == -1)   // tellg failed
        return 0;
    return len;
}

static bool isEmpty(std::ifstream &file)
{
    return file.peek() == std::ifstream::traits_type::eof();
}

void Response::createRedirectResponse(std::string &redirUrl, int statusCode, bool keepAlive)
{
    std::stringstream responseBuffer;

    Log(DBUG) << "Redirected to " << redirUrl << std::endl;
    responseBuffer << STATUS_LINE << getStatus(statusCode);
    Log(DBUG) << responseBuffer.str() << std::endl;
    responseBuffer << CRLF;
    responseBuffer << LOCATION << redirUrl << CRLF;
    if (keepAlive)
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << CONTENT_LEN << "0" << CRLF;
    responseBuffer << CRLF;
    _length = getStreamLen(responseBuffer);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    responseBuffer.read(_buffer, _length);
}

void Response::setResponseHeaders(std::stringstream &ss, Headers h)
{
    ss << STATUS_LINE << getStatus(h.statusCode);
    Log(DBUG) << ss.str() << std::endl;
    ss << CRLF;
    if (!h.contentType.empty())
        ss << CONTENT_TYPE << h.contentType << CRLF;
    ss << CONTENT_LEN << h.contentLen << CRLF;
    if (h.keepAlive)
        ss << KEEP_ALIVE << CRLF;
    ss << CRLF;
}

static Headers createHeaders(int statusCode, std::string contentType, size_t contentLen,
                             bool keepAlive)
{
    Headers h;

    h.statusCode = statusCode;
    h.contentType = contentType;
    h.contentLen = contentLen;
    h.keepAlive = keepAlive;

    return h;
}

void Response::setResponse(std::stringstream &ss)
{
    _length = getStreamLen(ss);
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    ss.read(_buffer, _length);
}

void Response::createGETResponse(Request &request)
{
    std::ifstream file;
    std::stringstream responseBuffer;
    std::string mimeType;
    size_t fileSize;

    file.open(request.resource().path.c_str(), std::ios::binary);
    if (!file.good())
        return createHTMLResponse(404, errorPage(404, request.resource()), false);
    mimeType = getContentType(request.resource().path);
    if (isEmpty(file))
        fileSize = 0;
    else
        fileSize = getStreamLen(file);
    setResponseHeaders(responseBuffer, createHeaders(200, mimeType, fileSize, request.keepAlive()));
    if (fileSize > 0)
        responseBuffer << file.rdbuf();
    file.close();

    setResponse(responseBuffer);
}

void Response::createFileResponse(Request &request, int statusCode)
{
    std::stringstream responseBuffer;
    std::ofstream file;
    file.open(request.resource().path.c_str());
    if (!file.good())
    {
        Log(ERR) << "Cannot open file to write: " << request.resource().path << std::endl;
        return createHTMLResponse(500, errorPage(500, request.resource()), false);
    }
    Log(DBUG) << "file being posted is " << request.resource().path << std::endl;
    file.write(request.buffer() + request.bodyStart(), request.length() - request.bodyStart());
    file.close();
    responseBuffer << STATUS_LINE << getStatus(statusCode);
    Log(DBUG) << responseBuffer.str() << std::endl;
    responseBuffer << CRLF;
    if (request.keepAlive())
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << LOCATION << request.resource().originalRequest << CRLF;
    responseBuffer << CRLF;
    setResponse(responseBuffer);
}

void Response::createDELETEResponse(Request &request)
{
    std::stringstream responseBuffer;
    int status;

    status = std::remove(request.resource().path.c_str());
    if (status != 0)
    {
        Log(ERR) << "Cannot delete file " << request.resource().path << std::endl;
        return createHTMLResponse(500, errorPage(500, request.resource()), false);
    }
    responseBuffer << STATUS_LINE << getStatus(204);
    Log(DBUG) << responseBuffer.str() << std::endl;
    responseBuffer << CRLF;
    if (request.keepAlive())
        responseBuffer << KEEP_ALIVE << CRLF;
    responseBuffer << CRLF;
    setResponse(responseBuffer);
}

void Response::createHTMLResponse(int statusCode, std::string page, bool keepAlive)
{
    std::stringstream responseBuffer;

    setResponseHeaders(responseBuffer, createHeaders(statusCode, HTML, page.length(), keepAlive));
    responseBuffer << page;
    setResponse(responseBuffer);
}

void Response::createHEADFileResponse(Request &request)
{
    std::ifstream file;
    std::stringstream responseBuffer;
    std::string mimeType;
    size_t fileSize;

    file.open(request.resource().path.c_str(), std::ios::binary);
    if (!file.good())
        return createHTMLResponse(404, errorPage(404, request.resource()), false);
    mimeType = getContentType(request.resource().path);
    if (isEmpty(file))
        fileSize = 0;
    else
        fileSize = getStreamLen(file);
    setResponseHeaders(responseBuffer, createHeaders(200, mimeType, fileSize, request.keepAlive()));
    file.close();
    setResponse(responseBuffer);
}

void Response::createHEADResponse(int statusCode, std::string contentType, bool keepAlive)
{
    std::stringstream responseBuffer;

    setResponseHeaders(responseBuffer, createHeaders(statusCode, contentType, 0, keepAlive));
    setResponse(responseBuffer);
}

static std::vector<char *>createExecArgs(std::string path)
{
    std::vector<char *> args;

    size_t fileStart = path.rfind("/");
    if (fileStart != std::string::npos)
        args.push_back(&path[fileStart + 1]);
    else
        args.push_back(strdup(path.c_str()));
    args.push_back(NULL);
    return args;
}

void Response::runCGI(Request &request, std::vector<char *> env)
{
    int status;
    int p[2][2];

    status = pipe(p[0]);
    if (status == -1)
        return createHTMLResponse(500, errorPage(500, request.resource()), request.keepAlive());
    status = pipe(p[1]);
    if (status == -1)
        return createHTMLResponse(500, errorPage(500, request.resource()), request.keepAlive());

    pid_t pid = fork();
    if (pid == -1)
        return createHTMLResponse(500, errorPage(500, request.resource()), request.keepAlive());
    if (pid == 0)
    {
        // close(p[0][1]);
        close(p[1][0]);
        dup2(p[0][0], STDIN_FILENO);
        close(p[0][0]);
        dup2(p[1][1], STDOUT_FILENO);
        close(p[1][1]);
        std::vector<char *> args = createExecArgs(request.resource().path);
        if (execve(request.resource().path.c_str(), &args[0], &env[0]) == -1)
        {
            // free everything, 500 error
            Log(ERR) << "Execve failed" << std::endl;
            createHTMLResponse(500, errorPage(500, request.resource()), request.keepAlive());
            exit(EXIT_FAILURE);
        }
    }
    else 
    {
        size_t totalBytes = 0;
        ssize_t bytesWritten;
        size_t bodySize = request.length() - request.bodyStart();
        const char *body = request.buffer() + request.bodyStart();

        while (totalBytes < bodySize)
        {
            // ? hopefully write will block here if the pipe fills up and wait until there is space in the buffer
            bytesWritten =  write(p[0][1], body + totalBytes, WRITE_SIZE(bodySize));
            if (bytesWritten == -1)
                return createHTMLResponse(500, errorPage(500, request.resource()), request.keepAlive());
            totalBytes += bytesWritten;
        }
        close(p[0][1]); // parent done writing to pipe
        Log(DBUG) << "WAITING FOR CHILD" << std::endl;
        waitpid(pid, NULL, 0);
        close(p[1][1]); // child done writing to pipe

        char buf[READ_MAX];
        std::string cgiOutput;
        cgiOutput = STATUS_LINE + getStatus(200);
        cgiOutput += CRLF;
        ssize_t bytesRead = read(p[1][0], buf, READ_MAX - 1);
        while (bytesRead > 0)
        {
            buf[bytesRead] = '\0';
            cgiOutput += buf;
            std::cout << "buf = " << buf << std::endl;
            bytesRead = read(p[1][0], buf, READ_MAX - 1);
        }
        close(p[1][0]);
        if (bytesRead == -1)
            return createHTMLResponse(500, errorPage(500, request.resource()), request.keepAlive());
        _length = cgiOutput.length();
        Log(DBUG) << "cgi output length = " << _length << " " << cgiOutput.length() << std::endl ;
        if (_buffer != NULL)
            delete[] _buffer;
        _buffer = new char[_length];
        std::copy(&cgiOutput[0], &cgiOutput[_length], _buffer);
    }

}

// void Response::createCGIResponse(std::vector<char *> env)
// {
//     pid_t pid = fork();
//     SystemCallException::checkErr("fork", pid); // ! change this to set 500 error in the response
//     if (pid == 0)
//     {

//     }

// }

Response::~Response()
{
    if (_buffer != NULL)
        delete[] _buffer;
}
