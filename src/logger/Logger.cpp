/**
 * @file Log.cpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Implementation of Log class
 * @date 2023-07-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "logger/Logger.hpp"

Logger::Logger(std::ostream& os) : _os(os), _color(WHITE)
{
}

Logger &Logger::operator()(const std::string &color)
{
    this->_color = color;
    return *this;
}

Logger &Logger::operator<<(Manipulator pf)
{
    _os << pf;
    return *this;
}

Logger& Logger::getLogger()
{
	static Logger log;
	return log;
}

std::ostream& Logger::os()
{
	return _os;
}

std::string& Logger::color()
{
	return _color;
}
