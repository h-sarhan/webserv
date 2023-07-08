/**
 * @file Server.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Class that describes a server instance adhering to the given configuration
 * @date 2023-07-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "network.hpp"
# include <map>
# include <exception>
# include <string>

class Server
{
	private:
		std::string	name;
		int	port;
		int	sockFd;
		// std::map<std::string route, Location location> locations;
		// std::map<int errCode, std::string pageLocation>	errorPages;

	public:
		Server();
		Server(std::string name, int port, int sockFd);
		Server(const Server &s);
		~Server();
};

#endif

