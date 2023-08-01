/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/28 18:10:53 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "cgiUtils.hpp"
#include "enums/HTTPMethods.hpp"
#include "enums/ResourceTypes.hpp"
#include "enums/conversions.hpp"
#include "logger/Logger.hpp"
#include "network/Server.hpp"
#include "requests/Request.hpp"
#include "responses/DefaultPages.hpp"
#include "responses/Response.hpp"
#include "utils.hpp"
#include <cstddef>

Connection::Connection()
    : _listener(-1), _request(), _response(), _keepAlive(false), _timeOut(0), _startTime(0),
      _dropped(false), _ip(), _reqReady(false)
{
}

Connection::Connection(int listener, std::string ip)
    : _listener(listener), _request(listener), _response(), _keepAlive(false), _timeOut(0),
      _startTime(0), _dropped(false), _ip(ip), _reqReady(false)
{
}

Connection::Connection(const Connection &c)
    : _listener(c._listener), _request(c._request), _response(c._response),
      _keepAlive(c._keepAlive), _timeOut(c._timeOut), _startTime(c._startTime),
      _dropped(c._dropped), _ip(c._ip), _reqReady(c._reqReady)
{
}

Connection &Connection::operator=(const Connection &c)
{
    if (this != &c)
    {
        this->_listener = c._listener;
        this->_request = c._request;
        this->_response = c._response;
        this->_keepAlive = c._keepAlive;
        this->_timeOut = c._timeOut;
        this->_startTime = c._startTime;
        this->_dropped = c._dropped;
        this->_ip = c._ip;
        this->_reqReady = c._reqReady;
    }
    return (*this);
}

int &Connection::listener()
{
    return _listener;
}

Request &Connection::request()
{
    return _request;
}

Response &Connection::response()
{
    return _response;
}

bool &Connection::keepAlive()
{
    return _keepAlive;
}

time_t &Connection::timeOut()
{
    return _timeOut;
}

time_t &Connection::startTime()
{
    return _startTime;
}

bool &Connection::dropped()
{
    return _dropped;
}

bool &Connection::reqReady()
{
    return _reqReady;
}

std::string Connection::ip()
{
    return _ip;
}

void Connection::processGET()
{
    Resource resource = _request.resource();
    Log(DBUG) << "Resource type: " << enumToStr(resource.type) << std::endl;
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createGETResponse(_request);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 302, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(200, directoryListing(resource), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createHTMLResponse(404, errorPage(404, resource), _keepAlive);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400, resource), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404, resource), _keepAlive);
        break;
    case CGI:
        _response.createCGIResponse(_request, prepCGIEnvironment());
        break;
    }
}

void Connection::processPOST()
{
    Resource resource = _request.resource();
    Log(DBUG) << "Resource type: " << enumToStr(resource.type) << std::endl;
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createHTMLResponse(409, errorPage(409, resource), _keepAlive);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 307, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createFileResponse(_request, 201);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400, resource), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404, resource), _keepAlive);
        break;
    case CGI:
        _response.createCGIResponse(_request, prepCGIEnvironment());
        break;
    }
}

void Connection::processPUT()
{
    Resource resource = _request.resource();
    Log(DBUG) << "Resource type: " << enumToStr(resource.type) << std::endl;
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createFileResponse(_request, 204);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 307, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createFileResponse(_request, 201);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400, resource), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404, resource), _keepAlive);
        break;
    case CGI:
        _response.createFileResponse(_request, 204);
        break;
    }
}

void Connection::processDELETE()
{
    Resource resource = _request.resource();
    Log(DBUG) << "Resource type: " << enumToStr(resource.type) << std::endl;
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createDELETEResponse(_request);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 307, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(405, errorPage(405, resource), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createHTMLResponse(404, errorPage(404, resource), _keepAlive);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400, resource), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404, resource), _keepAlive);
        break;
    case CGI:
        _response.createDELETEResponse(_request);
        break;
    }
}

void Connection::processHEAD()
{
    Resource resource = _request.resource();
    Log(DBUG) << "Resource type: " << enumToStr(resource.type) << std::endl;
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createHEADFileResponse(_request);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 302, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHEADResponse(405, NO_CONTENT, _keepAlive);
        break;
    case DIRECTORY:
        _response.createHEADResponse(200, HTML, _keepAlive);
        break;
    case NOT_FOUND:
        _response.createHEADResponse(404, NO_CONTENT, _keepAlive);
        break;
    case INVALID_REQUEST:
        _response.createHEADResponse(400, NO_CONTENT, _keepAlive);
        break;
    case NO_MATCH:
        _response.createHEADResponse(404, NO_CONTENT, _keepAlive);
        break;
    case CGI:
        _response.createCGIResponse(_request, prepCGIEnvironment());
        _response.trimBody();
        break;
    }
}

void Connection::processRequest()
{
    if (_request.length() == 0)
        return;
    _keepAlive = _request.keepAlive();
    _timeOut = _request.keepAliveTimer();
    if (bodySizeExceeded())
        return;
    switch (_request.method())
    {
    case GET:
        processGET();
        break;
    case POST:
        processPOST();
        break;
    case PUT:
        processPUT();
        break;
    case DELETE:
        processDELETE();
        break;
    case HEAD:
        processHEAD();
        break;
    case OTHER:
        _response.createHTMLResponse(400, errorPage(400, _request.resource()), _keepAlive);
        break;
    }
    _request.clear();
}

bool Connection::keepConnectionAlive()
{
    if (!_keepAlive)
        return false;
    _response.clear();
    time(&_startTime);   // reset timer
    return true;
}

bool Connection::bodySizeExceeded()
{
    size_t maxBodySize = _request.maxBodySize();
    if (_request.length() - _request.bodyStart() <= maxBodySize)
        return false;
    Log(ERR) << "Request body size exceeded limit! Size = " << _request.length()
             << ", Limit = " << maxBodySize << std::endl;
    if (_request.method() == HEAD)
        _response.createHEADResponse(413, NO_CONTENT, _keepAlive);
    else
        _response.createHTMLResponse(413, errorPage(413, _request.resource()), _keepAlive);
    _request.clear();
    return true;
}

std::vector<char *> Connection::prepCGIEnvironment()
{
    std::vector<char *> env;
    std::string var;

    env.push_back(strdup("SERVER_SOFTWARE=Webserv/1.1"));
    env.push_back(strdup("GATEWAY_INTERFACE=CGI/1.1"));
    env.push_back(strdup("SERVER_PROTOCOL=HTTP/1.1"));
    env.push_back(strdup("REQUEST_URI=/directory/youpi.bla"));
    addToEnv(env, "SERVER_NAME=" + _request.hostname());
    addToEnv(env, "SERVER_PORT=" + toStr(Server::getConfig(_request.listener())[0]->port));
    addToEnv(env, "REQUEST_METHOD=" + enumToStr(_request.method()));
    addToEnv(env, "REMOTE_ADDR=" + _ip);
    addPathEnv(env, _request.resource());
    addHeadersToEnv(env, _request.headers());
    return env;
}

Connection::~Connection()
{
}
