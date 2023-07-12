/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 17:34:41 by mfirdous          #+#    #+#             */
/*   Updated: 2023/07/12 19:26:43 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Connection.hpp"

Connection::Connection(int fd) : fdTemp(fd)
{
    // std::cout << "++++++++ " << fdTemp << "Connection made! ++++++++" << std::endl;
}

Connection::~Connection()
{
    // std::cout << "-------- " << fdTemp << "Connection destroyed --------" << std::endl;
}
