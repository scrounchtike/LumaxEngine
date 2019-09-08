
#include "Timing.hpp"

// Time and profiling methods
double lmx::getCurrentTimeMicroSeconds()
{
	auto time = std::chrono::high_resolution_clock::now();
	auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
	return time_us;
}

double lmx::getCurrentTimeMilliseconds()
{
	auto time = std::chrono::high_resolution_clock::now();
	auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
	return time_ms;
}
