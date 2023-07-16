/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/13 20:38:22 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"
#include "requests/Request.hpp"

Connection::Connection() : request()
{
}

Connection::Connection(int listener)
    : listener(listener), request(), response(), totalBytesRec(0), totalBytesSent(0)
{
}

Connection::Connection(const Connection &c)
    : listener(c.listener), request(c.request), response(c.response),
      totalBytesRec(c.totalBytesRec), totalBytesSent(c.totalBytesSent)
{
}

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
