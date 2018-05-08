
#include "Log.hpp"

#include <iostream>
#include "UsingDX11.hpp"

void Log::print(const std::string& string) {
#ifdef _USE_OPENGL
	std::cout << string << std::endl;
#elif defined _USE_DIRECTX11
	OutputDebugString(string.c_str());
#endif
}

void Log::println(const std::string& string) {
	print(string + "\n");
}

void Log::log(const std::string& string) {
	// TODO: Add log file + logs
}

void Log::logln(const std::string& string) {
	log(string + "\n");
}