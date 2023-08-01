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
#include "cgiUtils.hpp"
#include "logger/Logger.hpp"
#include "network/SystemCallException.hpp"
#include "responses/DefaultPages.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <libgen.h>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>

#define WRITE_MAX     65536
#define READ_MAX      1024
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
        return IDLE_CONNECTION;
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
    std::string filename = request.resource().path;

    // if (isDir(request.resource().path))
    //     filename += "/dirfile";
    file.open(filename.c_str());
    if (!file.good())
    {
        Log(ERR) << "Cannot open file to write: " << filename << std::endl;
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

void Response::trimBody()
{
    const char doubleCRLF[] = "\r\n\r\n";
    const char *bodyStart = std::search(_buffer, _buffer + _length, doubleCRLF,
                                        doubleCRLF + sizeOfArray(doubleCRLF) - 1);
    if (bodyStart != _buffer + _length)
        _length = bodyStart - _buffer + 4;
}

void Response::runCGI(int p[2], int outFd, Request &req, std::vector<char *> env)
{
    chdir(dirName(req.resource().path).c_str());
    close(p[1]);
    dup2(p[0], STDIN_FILENO);
    close(p[0]);
    dup2(outFd, STDOUT_FILENO);
    close(outFd);
    // std::string filename = "/Users/mfirdous/Desktop/Cursus/webserv/cgi_tester";

    std::string filename = "./";
    filename += baseName(req.resource().path);
    std::vector<char *> args = createExecArgs(filename);

    if (execve(filename.c_str(), &args[0], &env[0]) == -1)
    {
        std::for_each(env.begin(), env.end(), free);
        std::for_each(args.begin(), args.end(), free);
        Log(ERR) << "Execve failed" << std::endl;
        throw SystemCallException(std::string("execve: ") + strerror(errno) + filename);
    }
}

void Response::readCGIResponse(Request &req)
{
    Log(DBUG) << "Reading CGI response... " << std::endl;
    std::ifstream file;
    std::stringstream responseBuffer;
    size_t fileSize;

    responseBuffer << STATUS_LINE << getStatus(200) << CRLF;
    file.open(CGI_OUTFILE, std::ios::binary);
    if (!file.good())
        return createHTMLResponse(500, errorPage(500, req.resource()), req.keepAlive());
    if (isEmpty(file))
        fileSize = 0;
    else
        fileSize = getStreamLen(file);
    if (fileSize > 0)
        responseBuffer << file.rdbuf();
    file.close();
    std::remove(CGI_OUTFILE);
    setResponse(responseBuffer);
    Log(DBUG) << "Cgi output length = " << _length << std::endl;
}

int Response::sendCGIRequestBody(int pipeFd, Request &req)
{
    size_t totalBytes = 0;
    ssize_t bytesWritten;
    time_t startTime;
    time_t curTime;
    size_t bodySize = req.length() - req.bodyStart();
    const char *body = req.buffer() + req.bodyStart();

    if (fcntl(pipeFd, F_SETFL, O_NONBLOCK) == -1)
    {
        close(pipeFd);
        return 500;
    }
    time(&startTime);
    while (totalBytes < bodySize)
    {
        bytesWritten = write(pipeFd, body + totalBytes, WRITE_SIZE(bodySize - totalBytes));
        if (bytesWritten == -1)
        {
            time(&curTime);
            // gateway timed out because cgi must be idle and caused our pipe buffer to fill up
            if (curTime - startTime > GATEWAY_TIMEOUT)
            {
                Log(ERR) << "CGI Error (504): Pipe buf full for too long" << std::endl;
                close(pipeFd);
                return 504;
            }
        }
        else if (bytesWritten >= 0)
        {
            time(&startTime);   // reset start timer
            totalBytes += bytesWritten;
            Log(DBUG) << "bytesWritten = " << bytesWritten << ", total = " << totalBytes
                      << std::endl;
        }
    }
    close(pipeFd);   // parent done writing to input pipe
    return 0;
}

void Response::createCGIResponse(Request &req, std::vector<char *> env)
{
    int p[2];
    int status;
    int errCode;
    int outFd;

    pid_t pid = startCGIProcess(env, p, outFd);
    if (pid == -1)
        return createHTMLResponse(500, errorPage(500, req.resource()), req.keepAlive());
    if (pid == 0)
        runCGI(p, outFd, req, env);
    else
    {
        errCode = sendCGIRequestBody(p[1], req);
        Log(DBUG) << "WAITING FOR CHILD" << std::endl;
        pid_t waitStatus = waitCGI(pid, status, errCode);
        close(p[0]);   // child done reading from input pipe
        close(outFd); // child done writing to output pipe
        std::for_each(env.begin(), env.end(), free);
        errCode = checkCGIError(pid, errCode, waitStatus, status);
        if (errCode != EXIT_SUCCESS)
        {
            std::remove(CGI_OUTFILE);
            return createHTMLResponse(errCode, errorPage(errCode, req.resource()), req.keepAlive());
        }
        readCGIResponse(req);
    }
}

Response::~Response()
{
    if (_buffer != NULL)
        delete[] _buffer;
}
