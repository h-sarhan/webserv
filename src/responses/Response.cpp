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
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <sys/poll.h>
// #include <sys/syslimits.h>
#include <unistd.h>
#include "network/SystemCallException.hpp"
#include <libgen.h>

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
    args.push_back(strdup(path.c_str()));
    args.push_back(NULL);
    return args;
}

static pid_t startCGIProcess(int p[2][2])
{
    if (pipe(p[0]) == -1)
        return -1;
    if (pipe(p[1]) == -1)
        return -1;
    pid_t pid = fork();
    if (pid == -1)
        return -1;
    return pid;
}

void Response::runCGI(int p[2][2], Request &req, std::vector<char *> env)
{
    chdir(dirName(req.resource().path).c_str());
    // Log(DBUG) << "cwd = " << getcwd(NULL, 0) << std::endl;
    close(p[1][0]);
    dup2(p[0][0], STDIN_FILENO);
    close(p[0][0]);
    dup2(p[1][1], STDOUT_FILENO);
    close(p[1][1]);
    // std::string filename = "cgi_tester";
    // std::string filename = "./";
    std::string filename = baseName(req.resource().path);
    std::vector<char *> args = createExecArgs(filename);

    if (execve(filename.c_str(), &args[0], &env[0]) == -1)
    {
        std::for_each(env.begin(), env.end(), free);
        std::for_each(args.begin(), args.end(), free);
        close(p[0][1]);
        Log(ERR) << "Execve failed" << std::endl;
        createHTMLResponse(500, errorPage(500, req.resource()), req.keepAlive());
        // check if this actually returns shows a 500 error to the client
        throw SystemCallException(std::string("execve failed! ") + strerror(errno) + filename);
        // ! maybe check wifexited or something
    }
}

void Response::sendCGIRequestBody(int pipeFd, Request &req)
{
    size_t totalBytes = 0;
    ssize_t bytesWritten;
    size_t bodySize = req.length() - req.bodyStart();
    const char *body = req.buffer() + req.bodyStart();

    while (totalBytes < bodySize)
    {
        // check if child proc is still alive before writing
        // * write will block here if the pipe fills up and wait until there is space in the buffer
        bytesWritten =  write(pipeFd, body + totalBytes, WRITE_SIZE(bodySize - totalBytes));
        if (bytesWritten == -1)
        {
            Log(ERR) << "Could not write to CGI pipe" << std::endl;
            return createHTMLResponse(500, errorPage(500, req.resource()), req.keepAlive());
        }
        totalBytes += bytesWritten;
        Log(DBUG) << "bytesWritten = " << bytesWritten << ", total = " << totalBytes << std::endl;
    }
    close(pipeFd); // parent done writing to input pipe
}

void Response::readCGIResponse(int pipeFd, Request &req)
{
    char buf[READ_MAX];
    std::string cgiOutput;
    ssize_t bytesRead;

    cgiOutput = STATUS_LINE + getStatus(200) + CRLF;
    // cgiOutput += CRLF;
    bytesRead = read(pipeFd, buf, READ_MAX - 1);
    while (bytesRead > 0)
    {
        buf[bytesRead] = '\0';
        // cgiOutput += buf;
        cgiOutput += std::string(buf, bytesRead);
        bytesRead = read(pipeFd, buf, READ_MAX - 1);
    }
    close(pipeFd); // parent done reading from output pipe
    if (bytesRead == -1)
    {
        Log(ERR) << "Could not read CGI output from pipe" << std::endl;
        return createHTMLResponse(500, errorPage(500, req.resource()), req.keepAlive());
    }
    _length = cgiOutput.length();
    Log(DBUG) << "Cgi output length = " << _length << " " << cgiOutput.length() << std::endl ;
    // Log(DBUG) << cgiOutput << std::endl;
    if (_buffer != NULL)
        delete[] _buffer;
    _buffer = new char[_length];
    std::copy(&cgiOutput[0], &cgiOutput[_length], _buffer);
}

void Response::createCGIResponse(Request &req, std::vector<char *> env)
{
    int p[2][2];
    
    pid_t pid = startCGIProcess(p);
    if (pid == -1)
        return createHTMLResponse(500, errorPage(500, req.resource()), req.keepAlive());
    if (pid == 0)
        runCGI(p, req, env);
    else 
    {
        sendCGIRequestBody(p[0][1], req);
        Log(DBUG) << "WAITING FOR CHILD" << std::endl;
        waitpid(pid, NULL, 0);
        close(p[0][0]); // child done reading from input pipe
        close(p[1][1]); // child done writing to output pipe
        std::for_each(env.begin(), env.end(), free);
        readCGIResponse(p[1][0], req);
    }
}

Response::~Response()
{
    if (_buffer != NULL)
        delete[] _buffer;
}
