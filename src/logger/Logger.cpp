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

Logger::Logger(std::ostream& os) : _os(os), _color(RESET)
{
}

Logger &Logger::operator()(const std::string &color)
{
    this->_color = color;
	_os << color << getTimeStamp();
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

std::string Logger::getTimeStamp()
{
	time_t curTime;
    time(&curTime);
	std::stringstream ss;

    int clock = curTime - 18000;
    int secsPastMidnight = clock % 86400;
    int hours = (secsPastMidnight / 3600) + 9;
    int minutes = (secsPastMidnight % 3600) / 60;
    int seconds = secsPastMidnight % 60;
	ss << "[" << hours << ":" << minutes << ":" << seconds << "]" << "\t";
	return ss.str();
}

std::ostream& Logger::os()
{
	return _os;
}

std::string& Logger::color()
{
	return _color;
}
