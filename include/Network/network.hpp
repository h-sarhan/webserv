/**
 * @file network.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief This file contains all the necessary headers for networking
 * @date 2023-07-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef NETWORK_HPP
# define NETWORK_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <stdlib.h>
# include <cstring>
# include <errno.h>
# include <unistd.h>

# define QUEUE_LIMIT 10
# define PORT "1234"

#endif
