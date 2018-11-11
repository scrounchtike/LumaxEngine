
#ifndef LOG_HPP
#define LOG_HPP

#include <string>

#include "buildDesc.hpp"

class Log {
public:
	static void print(const std::string& string);
	static void println(const std::string& string);
	static void log(const std::string& string);
	static void logln(const std::string& string);
private:
};

#endif