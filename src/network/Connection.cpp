/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/22 13:57:58 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "enums/HTTPMethods.hpp"
#include "enums/ResourceTypes.hpp"
#include "enums/conversions.hpp"
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

static void showResourceInfo(Resource &resource, Request &request)
{
    std::cout << "method type: " << enumToStr(request.method()) << std::endl;
    std::cout << resource.originalRequest << " resource found at: " << resource.path << std::endl;
    std::cout << "request type: " << enumToStr(resource.type) << std::endl;
}

void Connection::processGET(configList config)
{
    Resource resource = request.resource(config);
    showResourceInfo(resource, request);
    switch (resource.type)
    {
    case EXISTING_FILE:
        response.createGETResponse(resource.path, keepAlive);
        break;
    case REDIRECTION:
        response.createRedirectResponse(resource.path, 302, keepAlive);
        break;
    case FORBIDDEN_METHOD:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case DIRECTORY:
        response.createHTMLResponse(200, directoryListing(resource), keepAlive);
        break;
    case NOT_FOUND:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    case INVALID_REQUEST:
        response.createHTMLResponse(400, errorPage(400), keepAlive);
        break;
    case NO_MATCH:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    }
}

void Connection::processPOST(configList config)
{
    Resource resource = request.resource(config);
    showResourceInfo(resource, request);
    switch (resource.type)
    {
    case EXISTING_FILE:
        response.createHTMLResponse(409, errorPage(409), keepAlive);
        break;
    case REDIRECTION:
        response.createRedirectResponse(resource.path, 307, keepAlive);
        break;
    case FORBIDDEN_METHOD:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case DIRECTORY:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case NOT_FOUND:
        response.createFileResponse(resource.path, request, 201);
        break;
    case INVALID_REQUEST:
        response.createHTMLResponse(400, errorPage(400), keepAlive);
        break;
    case NO_MATCH:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    }
}

void Connection::processPUT(configList config)
{
    Resource resource = request.resource(config);
    showResourceInfo(resource, request);
    switch (resource.type)
    {
    case EXISTING_FILE:
        response.createFileResponse(resource.path, request, 204);
        break;
    case REDIRECTION:
        response.createRedirectResponse(resource.path, 307, keepAlive);
        break;
    case FORBIDDEN_METHOD:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case DIRECTORY:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case NOT_FOUND:
        response.createFileResponse(resource.path, request, 201);
        break;
    case INVALID_REQUEST:
        response.createHTMLResponse(400, errorPage(400), keepAlive);
        break;
    case NO_MATCH:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    }
}

void Connection::processDELETE(configList config)
{
    Resource resource = request.resource(config);
    showResourceInfo(resource, request);
    switch (resource.type)
    {
    case EXISTING_FILE:
        response.createDELETEResponse(resource.path, request);
        break;
    case REDIRECTION:
        response.createRedirectResponse(resource.path, 307, keepAlive);
        break;
    case FORBIDDEN_METHOD:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case DIRECTORY:
        response.createHTMLResponse(405, errorPage(405), keepAlive);
        break;
    case NOT_FOUND:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    case INVALID_REQUEST:
        response.createHTMLResponse(400, errorPage(400), keepAlive);
        break;
    case NO_MATCH:
        response.createHTMLResponse(404, errorPage(404), keepAlive);
        break;
    }
}

void Connection::processHEAD(configList config)
{
    Resource resource = request.resource(config);
    showResourceInfo(resource, request);
    switch (resource.type)
    {
    case EXISTING_FILE:
        response.createHEADFileResponse(resource.path, request);
        break;
    case REDIRECTION:
        response.createRedirectResponse(resource.path, 302, keepAlive);
        break;
    case FORBIDDEN_METHOD:
        response.createHEADResponse(405, NO_CONTENT, keepAlive);
        break;
    case DIRECTORY:
        response.createHEADResponse(200, HTML, keepAlive);
        break;
    case NOT_FOUND:
        response.createHEADResponse(404, NO_CONTENT, keepAlive);
        break;
    case INVALID_REQUEST:
        response.createHEADResponse(400, NO_CONTENT, keepAlive);
        break;
    case NO_MATCH:
        response.createHEADResponse(404, NO_CONTENT, keepAlive);
        break;
    }
}

void Connection::processRequest(configList config)
{
    if (request.length() == 0)
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
        processDELETE(config);
        break;
    case HEAD:
        processHEAD(config);
        break;
    case OTHER:
        response.createHTMLResponse(400, errorPage(400), keepAlive);
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
