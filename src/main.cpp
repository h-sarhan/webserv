/**
 * @file main.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief The entrypoint to our program
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include "network.hpp"
#include "common.hpp"

/**
 * @brief Entrypoint to our program
 *
 * @return int Exit code
 */
int main(int argc, char **argv)
{
	// struct addrinfo *servInfo;
	// struct addrinfo hints;
	// int status;

	// // std::fill(&hints, &hints + sizeof(hints), 0); // cpp alternative for memset, yet to test
	// memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
	// hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	// hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	// hints.ai_flags = AI_PASSIVE; // fill in my IP for me

	// if ((status = getaddrinfo(NULL, "1234", &hints, &servInfo)) != 0) // first param is host IP/name and its null because we set ai_flags as AI_PASSIVE
	// {
	// 	std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl; // make this an exception or make a common error class later
	// 	exit(1);
	// }
	// std::cout << "getaddrinfo successful!" << std::endl;
	// freeaddrinfo(servInfo);
	// return (0);

	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	if (argc != 2) {
		std::cerr << "usage: showip hostname" << std::endl;
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return 2;
	}

	std::cout << "IP addresses for: " << argv[1] << std::endl << std::endl;

	for(p = res;p != NULL; p = p->ai_next) 
	{
		void *addr;
		std::string ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		std::cout << ipver.c_str() << " : " << ipstr << std::endl;
	}

	freeaddrinfo(res); // free the linked list

	return 0;
}
