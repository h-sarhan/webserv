#include <exception>
#include <string>
#include <iostream>

class SystemCallException : public std::exception
{
	private:
		std::string errorMessage;

	public:
		SystemCallException(const std::string& functionName, const std::string& errorMsg) 
		: errorMessage(functionName + ": " + errorMsg) {}
		SystemCallException(const std::string& errorMsg) 
		: errorMessage("webserv: " + errorMsg) {}

		virtual const char* what() const throw()
		{
			return errorMessage.c_str();
		}
		~SystemCallException() throw() {}
};
