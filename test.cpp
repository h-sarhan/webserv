// #include <iostream>
#include <utility>


// class logger {
// public:

//     logger( std::ostream& os_, std::string color_ = WHITE );

//     //this allows you to do things like :
//     //logger log(std::cout);
//     //log(123);
//     //and it returns a logger class so you can chain << to this as well. (read below for
//     explanation) logger& operator()(std::string color_); logger& operator()(std::ostream&
//     (*pf)(std::ostream&)); // Overload for manipulators void setColor(std::string color_);

// 	friend logger& operator<<(std::ostream& (*pf)(std::ostream&));

//     //functions declared with the friend keyword are not member functions,
//     //but can see private members of the enclosing class
//     template<class T>
//     friend logger& operator<<( logger& log, const T& output );

// private:

//     std::ostream& os;
//     std::string color;

// };

// logger::logger( std::ostream& os_, std::string color_ ) : os(os_), color(color_) {}

// logger& logger::operator()(std::string color_) {
//     setColor(color_);
//     return *this;
// }

// void logger::setColor(std::string color_) {
//     color = color_;
// }

// logger& logger::operator<<(std::ostream& (*pf)(std::ostream&)) {
//     os << pf; // Apply the manipulator to the output stream
//     return *this;
// }
// //return type is logger& because you want to be able to chain <<() calls : (log << text1 <<
// numbers << blahblah;)
// //so :
// //(log) has the type logger, so we can call operator<<()
// //(log << text1) also has the type logger, we still can call operator<<() ( in fact it is the
// same logger instance (log) )
// //and so on..
// template<class T>
// logger& operator<<( logger& log, const T& output ) {
//     //see? normal function, but has access to log.Color and log.os because of friend declaration
//     above log.os << log.color << output << RESET; return log;
// }

#include <iostream>
#include <sstream>

// #define RED    "\x1b[31m"
// #define YELLOW "\x1b[33m"
// #define RESET  "\x1b[0m"

#define ERR RED
#define WARN YELLOW
#define INFO BLUE
#define SUCCESS GREEN
#define RED        "\x1b[31m"
#define WHITE      "\x1b[37m"
#define BLUE       "\x1b[34m"
#define YELLOW     "\x1b[33m"
#define PURPLE     "\x1b[35m"
#define CYAN       "\x1b[36m"
#define GREEN      "\x1B[32m"
#define BOLD       "\x1b[1m"
#define RESET      "\x1b[0m"
#define WHITESPACE " \t\n\r\f\v"

class Logger
{
  private:
    std::ostream &_os;
    std::string _color;
  public:
    Logger();
    Logger &operator()(const std::string &color);

    typedef std::ostream &(*Manipulator)(std::ostream &);
    Logger &operator<<(Manipulator pf);

    template <class T> friend Logger &operator<<(Logger &log, const T &output);

};

Logger::Logger() : _os(std::cout), _color(WHITE)
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

template <class T> Logger &operator<<(Logger &log, const T &output)
{
    log._os << log._color << output << RESET;
    return log;
}

int main()
{
    Logger log;

    float f = 5.34344f;
    log << "Hi";
    log(ERR) << "Hello"
             << " " << 324 << f << std::endl;
	log(INFO) << "CHECK THIS OUT" << std::endl << std::endl;
    log(WARN) << "Hello" << 902348023 << std::endl;
	log(SUCCESS) << "SENT" << std::endl;
    return 0;
}
