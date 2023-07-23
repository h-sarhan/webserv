/**
 * @file Log.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Logger to print custom log messages with different categories: Error, Info, Warning
 * @date 2023-07-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <iostream>
#include <sstream>
#include <utility>
#include "utils.hpp"

#define ERR RED
#define DBUG BLUE
#define WARN YELLOW
#define INFO WHITE
#define SUCCESS GREEN

typedef std::ostream &(*Manipulator)(std::ostream &);

class Logger
{
  private:
    std::ostream &_os; // output stream for logs, default 
    std::string _color;
    std::string getTimeStamp();

  public:
    Logger(std::ostream &os = std::cout);
    Logger &operator()(const std::string &color);
    Logger &operator<<(Manipulator pf);
    static Logger &getLogger();
    std::ostream& os();
    std::string& color();
};

template <class T> Logger &operator<<(Logger &log, const T &output)
{
    log.os() << log.color() << output << RESET;
    return log;
}

static Logger& log = Logger::getLogger();

#endif
