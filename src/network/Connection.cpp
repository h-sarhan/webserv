/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/22 20:44:53 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "enums/HTTPMethods.hpp"
#include "enums/ResourceTypes.hpp"
#include "enums/conversions.hpp"
#include "requests/Request.hpp"
#include "responses/DefaultPages.hpp"
#include "responses/Response.hpp"
#include "logger/Logger.hpp"

Connection::Connection()
    : _listener(-1), _request(), _response(), _keepAlive(false), _timeOut(0), _startTime(0)
{
}

Connection::Connection(int listener)
    : _listener(listener), _request(listener), _response(), _keepAlive(false), _timeOut(0), _startTime(0)
{
}

Connection::Connection(const Connection &c)
    : _listener(c._listener), _request(c._request), _response(c._response), _keepAlive(c._keepAlive),
      _timeOut(c._timeOut), _startTime(c._startTime)
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
    }
    return (*this);
}

int& Connection::listener()
{
    return _listener;
}

Request& Connection::request()
{
    return _request;
}

Response& Connection::response()
{
    return _response;
}

bool& Connection::keepAlive()
{
    return _keepAlive;
}

time_t& Connection::timeOut()
{
    return _timeOut;
}

time_t& Connection::startTime()
{
    return _startTime;
}

void Connection::showResourceInfo(Resource &resource)
{
    log(DBUG) << "Method type: " << enumToStr(_request.method()) << std::endl;
    log(DBUG) << "Resource type: " << enumToStr(resource.type) << std::endl;
    // log(DBUG) << resource.originalRequest << " resource found at: <" << resource.path << ">" << std::endl;
}

void Connection::processGET()
{
    Resource resource = _request.resource();
    showResourceInfo(resource);
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createGETResponse(resource.path, _keepAlive);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 302, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(200, directoryListing(resource), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createHTMLResponse(404, errorPage(404), _keepAlive);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404), _keepAlive);
        break;
    }
}

void Connection::processPOST()
{
    Resource resource = _request.resource();
    showResourceInfo(resource);
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createHTMLResponse(409, errorPage(409), _keepAlive);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 307, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createFileResponse(resource.path, _request, 201);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404), _keepAlive);
        break;
    }
}

void Connection::processPUT()
{
    Resource resource = _request.resource();
    showResourceInfo(resource);
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createFileResponse(resource.path, _request, 204);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 307, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createFileResponse(resource.path, _request, 201);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404), _keepAlive);
        break;
    }
}

void Connection::processDELETE()
{
    Resource resource = _request.resource();
    showResourceInfo(resource);
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createDELETEResponse(resource.path, _request);
        break;
    case REDIRECTION:
        _response.createRedirectResponse(resource.path, 307, _keepAlive);
        break;
    case FORBIDDEN_METHOD:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case DIRECTORY:
        _response.createHTMLResponse(405, errorPage(405), _keepAlive);
        break;
    case NOT_FOUND:
        _response.createHTMLResponse(404, errorPage(404), _keepAlive);
        break;
    case INVALID_REQUEST:
        _response.createHTMLResponse(400, errorPage(400), _keepAlive);
        break;
    case NO_MATCH:
        _response.createHTMLResponse(404, errorPage(404), _keepAlive);
        break;
    }
}

void Connection::processHEAD()
{
    Resource resource = _request.resource();
    showResourceInfo(resource);
    switch (resource.type)
    {
    case EXISTING_FILE:
        _response.createHEADFileResponse(resource.path, _request);
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
    }
}

void Connection::processRequest()
{
    if (_request.length() == 0)
        return;
    _keepAlive = _request.keepAlive();
    _timeOut = _request.keepAliveTimer();
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
        _response.createHTMLResponse(400, errorPage(400), _keepAlive);
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

Connection::~Connection()
{
}
