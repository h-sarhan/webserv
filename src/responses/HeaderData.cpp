/**
 * @file HeaderData.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of header data util functions
 * @date 2023-07-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "responses/HeaderData.hpp"

std::string getStatus(int statusCode)
{
	switch (statusCode)
	{
		case 200:
			return "200 OK";
		case 201:
			return "201 Created";
		case 302:
			return "302 Found";
		case 404:
			return "404 Not found";
		case 405:
			return "405 Method not allowed";
		case 503:
			return "503 Service Unavailable";
	}
	return "500 Internal Server Error";
}

std::string getContentType(std::string extension);
