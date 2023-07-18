/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/17 16:18:23 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "requests/Request.hpp"

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

Connection::~Connection()
{
}
