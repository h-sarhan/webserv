/**
 * @file Log.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief Logger to print custom log messages with different categories: Error, Info, Warning
 * @date 2023-07-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <sstream>
#include <utility>
#include "utils.hpp"

#define ERR RED
#define WARN YELLOW
#define INFO BLUE
#define SUCCESS GREEN

// class Logger
// {
//   private:
//     std::ostream &_os;
//     std::string _color;
//   public:
//     Logger();
//     Logger &operator()(const std::string &color);

//     typedef std::ostream &(*Manipulator)(std::ostream &);
//     Logger &operator<<(Manipulator pf);

//     template <class T> friend Logger &operator<<(Logger &log, const T &output);

// };

// Logger::Logger() : _os(std::cout), _color(WHITE)
// {
// }

// Logger &Logger::operator()(const std::string &color)
// {
//     this->_color = color;
//     return *this;
// }

// Logger &Logger::operator<<(Manipulator pf)
// {
//     _os << pf;
//     return *this;
// }

// template <class T> Logger &operator<<(Logger &log, const T &output)
// {
//     log._os << log._color << output << RESET;
//     return log;
// }
typedef std::ostream &(*Manipulator)(std::ostream &);

class Logger
{
  private:
    std::ostream &_os; // output stream for logs, default 
    std::string _color;

  public:
    Logger(std::ostream &os = std::cout);
    Logger &operator()(const std::string &color);
    Logger &operator<<(Manipulator pf);
    std::ostream& os();
    std::string& color();
};

template <class T> Logger &operator<<(Logger &log, const T &output)
{
    log.os() << log.color() << output << RESET;
    return log;
}
