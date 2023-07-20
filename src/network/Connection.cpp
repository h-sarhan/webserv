/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/19 22:21:43 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "enums/HTTPMethods.hpp"
#include "requests/Request.hpp"
#include "responses/DefaultPages.hpp"

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

// static std::string createHTMLResponse(std::string page, std::string headers)
// {
//     std::stringstream responseBuffer;

//     responseBuffer << headers;
//     responseBuffer << page.length() << "\r\n\r\n";
//     responseBuffer << page;
//     return responseBuffer.str();
// }

// static std::string createResponse(std::string filename, std::string headers)
// {
//     // std::ifstream file;
//     // std::stringstream responseBuffer;
//     // std::stringstream fileBuffer;
//     // std::string fileContents;

//     // file.open(filename.c_str());
//     // // if (!file.good())
//     // // return 404 page as response
//     // fileBuffer << file.rdbuf();
//     // file.close();
//     // fileContents = fileBuffer.str();
//     // return createHTMLResponse(fileContents, headers);
// }

std::string Connection::createResponseHeaders()
{
    std::string headers = HTTP_HEADERS;
    if (keepAlive)
        headers += KEEP_ALIVE;
    return headers;
}

void Connection::processGet(std::vector<ServerBlock *> &config)
{
    std::string headers;

    RequestTarget target = request.target(config);
    std::cout << "method type: " << request.method() << std::endl;
    std::cout << request.rawTarget() << " resource found at: " << target.resource << std::endl;
    std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
    keepAlive = request.keepAlive();
    timeOut = request.keepAliveTimer();
    headers = createResponseHeaders();
    switch (target.type)
    {
    case FOUND:
        response.createResponse(target.resource, headers);
        break;
    case REDIRECTION:
        response.createRedirectResponse(target.resource, keepAlive);
        break;
    case METHOD_NOT_ALLOWED:
        response.createHTMLResponse(errorPage(405));
        break;
    case DIRECTORY:
        response.createHTMLResponse(directoryListing(target.resource));
        break;
    case NOT_FOUND:
        response.createHTMLResponse(errorPage(404));
        break;
    }

}

void Connection::processRequest(std::vector<ServerBlock *> &config)
{
    std::string headers;
    if (request.requestLength() == 0)
        return;
    switch (request.method())
    {
        case GET:
            break;
        case POST:
            break;
        case PUT:
            break;
        case DELETE:
            break;
        case OTHER:
            break;
    }
    RequestTarget target = request.target(config);
    std::cout << "method type: " << request.method() << std::endl;
    std::cout << request.rawTarget() << " resource found at: " << target.resource << std::endl;
    std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
    keepAlive = request.keepAlive();
    timeOut = request.keepAliveTimer();
    headers = createResponseHeaders();
    switch (target.type)
    {
    case FOUND:
        response.createResponse(target.resource, headers);
        break;
    case REDIRECTION:
        response.createRedirectResponse(target.resource, keepAlive);
        break;
    case METHOD_NOT_ALLOWED:
        response.createHTMLResponse(405, errorPage(405));
        break;
    case DIRECTORY:
        response.createHTMLResponse(200, directoryListing(target.resource));
        break;
    case NOT_FOUND:
        response.createHTMLResponse(404, errorPage(404));
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
