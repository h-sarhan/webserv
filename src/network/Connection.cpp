/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/20 19:36:07 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "enums/HTTPMethods.hpp"
#include "enums/RequestTypes.hpp"
#include "requests/Request.hpp"
#include "responses/DefaultPages.hpp"
#include "responses/Response.hpp"

Connection::Connection()
    : listener(-1), request(), response(), keepAlive(false), timeOut(0), startTime(0)
{
}

Connection::Connection(int listener)
    : listener(listener), request(), response(), keepAlive(false), timeOut(0), startTime(0)
{
}

Connection::Connection(const Connection &c)
    : listener(c.listener), request(c.request), response(c.response), keepAlive(c.keepAlive),
      timeOut(c.timeOut), startTime(c.startTime)
{
}

Connection &Connection::operator=(const Connection &c)
{
    if (this != &c)
    {
        this->listener = c.listener;
        this->request = c.request;
        this->response = c.response;
        this->keepAlive = c.keepAlive;
        this->timeOut = c.timeOut;
        this->startTime = c.startTime;
    }
    return (*this);
}

std::string Connection::createResponseHeaders()
{
    std::string headers = HTTP_HEADERS;
    if (keepAlive)
        headers = headers + KEEP_ALIVE + CRLF;
    return headers;
}

void Connection::processGET(std::vector<ServerBlock *> &config)
{
    std::string headers;

    RequestTarget target = request.target(config);
    std::cout << "method type: " << request.method() << std::endl;
    std::cout << request.rawTarget() << " resource found at: " << target.resource << std::endl;
    std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
    switch (target.type)
    {
    case FOUND:
        response.createGETResponse(target.resource, keepAlive);
        break;
    case REDIRECTION:
        response.createRedirectResponse(target.resource, 302, keepAlive);
        break;
    case METHOD_NOT_ALLOWED:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case DIRECTORY:
        response.createHTMLResponse(200, directoryListing(target.resource), keepAlive);
        break;
    case NOT_FOUND:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    }
}

void Connection::processPOST(std::vector<ServerBlock *> &config)
{
    RequestTarget target = request.target(config);
    std::cout << "method type: " << httpMethodtoStr(request.method()) << std::endl;
    std::cout << request.rawTarget() << " resource found at: " << target.resource << std::endl;
    std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
    switch (target.type)
    {
    case FOUND:
        response.createHTMLResponse(409, errorPage(409), keepAlive);
        break;
    case REDIRECTION:
        response.createRedirectResponse(target.resource, 307, keepAlive);
        break;
    case METHOD_NOT_ALLOWED:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case DIRECTORY:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case NOT_FOUND:
        response.createPOSTResponse(request.rawTarget(), request);
    }
}

void Connection::processPUT(std::vector<ServerBlock *> &config)
{
    std::string headers;

    RequestTarget target = request.target(config);
    std::cout << "method type: " << request.method() << std::endl;
    std::cout << request.rawTarget() << " resource found at: " << target.resource << std::endl;
    std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
    switch (target.type)
    {
    case FOUND:
        response.createHTMLResponse(409, errorPage(409), keepAlive);
        break;
    case METHOD_NOT_ALLOWED:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    default:
        // response.createPOSTResponse(target.resource, request);
        response.createPOSTResponse(request.rawTarget(), request);
    }
}

void Connection::processRequest(std::vector<ServerBlock *> &config)
{
    if (request.requestLength() == 0)
        return;
    keepAlive = request.keepAlive();
    timeOut = request.keepAliveTimer();
    switch (request.method())
    {
        case GET:
            processGET(config);
            break;
        case POST:
            processPOST(config);
            break;
        case PUT:
            processPUT(config);
            break;
        case DELETE:
            break;
        case OTHER:
            break;
    }
    request.clear();
}

bool Connection::keepConnectionAlive()
{
    if (!keepAlive)
        return false;
    std::cout << "Connection is keep alive" << std::endl;
    response.clear();
    time(&startTime);   // reset timer
    return true;
}

Connection::~Connection()
{
}
