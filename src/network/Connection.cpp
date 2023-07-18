/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/18 18:02:13 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "requests/Request.hpp"
#include "responses/DefaultPages.hpp"

Connection::Connection()
    : listener(-1), request(), response(), totalBytesRec(0), totalBytesSent(0)
{
}
// Connection::Connection()
//     : listener(-1), request(), response(), totalBytesRec(0), totalBytesSent(0), keepAlive(false),
//       timeOut(0), startTime(0)
// {
// }

Connection::Connection(int listener)
    : listener(listener), request(), response(), totalBytesRec(0), totalBytesSent(0)
{
}
// Connection::Connection(int listener)
//     : listener(listener), request(), response(), totalBytesRec(0), totalBytesSent(0),
//       keepAlive(false), timeOut(0), startTime(0)
// {
// }

Connection::Connection(const Connection &c)
    : listener(c.listener), request(c.request), response(c.response),
      totalBytesRec(c.totalBytesRec), totalBytesSent(c.totalBytesSent)
{
}
// Connection::Connection(const Connection &c)
//     : listener(c.listener), request(c.request), response(c.response),
//       totalBytesRec(c.totalBytesRec), totalBytesSent(c.totalBytesSent), keepAlive(c.keepAlive),
//       timeOut(c.timeOut), startTime(c.startTime)
// {
// }

Connection &Connection::operator=(const Connection &c)
{
    if (this != &c)
    {
        this->listener = c.listener;
        this->request = c.request;
        this->response = c.response;
        this->totalBytesRec = c.totalBytesRec;
        this->totalBytesSent = c.totalBytesSent;
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

//     std::ifstream file;
//     std::stringstream responseBuffer;
//     std::string fileContents;

//     file.open(filename.c_str(), std::ios::binary);
//     if (!file.good())
//         return createHTMLResponse(errorPage(404), headers);
//     file.seekg(0, std::ios::end);
//     std::ios_base::streampos fileLen = file.tellg();
//     file.seekg(0, std::ios::beg);
//     responseBuffer << headers;
//     responseBuffer << fileLen << "\r\n\r\n";
//     responseBuffer << file.rdbuf();
//     file.close();
//     fileContents = responseBuffer.str();
//     return fileContents;
// }

void Connection::processRequest(std::vector<ServerBlock *>& config)
{
    if (request.requestLength() == 0)
        return;
    RequestTarget target = request.target(config);
    std::cout << "resource found at: " << target.resource << std::endl;
    std::cout << "request type: " << requestTypeToStr(target.type) << std::endl;
    switch (target.type)
    {
        case FOUND:
            response.createResponse(target.resource, HTTP_HEADERS);
            break;
        case REDIRECTION:
            response.createResponse(target.resource, HTTP_HEADERS);
            break;
        case METHOD_NOT_ALLOWED:
            response.createHTMLResponse(errorPage(405), HTTP_HEADERS);
            break;
        case DIRECTORY:
            response.createHTMLResponse(directoryListing(target.resource), HTTP_HEADERS);
            break;
        case NOT_FOUND:
            response.createHTMLResponse(errorPage(404), HTTP_HEADERS);
            break;
    }
    // keepAlive = request.keepAlive();
    // timeOut = request.keepAliveTimer();
    totalBytesSent = 0;
    request.clear();
}

Connection::~Connection()
{
}
