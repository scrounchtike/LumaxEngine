
#ifndef TIME_HPP
#define TIME_HPP

#include "../RAL/buildDesc.hpp"

#ifdef _WINDOWS

#include "../math/types.hpp"
#include <windows.h>
#include <chrono>

namespace Time {
	static float64 frequency;

	void initTimer() {
		LARGE_INTEGER cfreq;
		QueryPerformanceCounter(&cfreq);

		frequency = (float64)cfreq.QuadPart;
	}

	float64 getTime() {
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);

		return 1000.0 * (float64)t.QuadPart / frequency;
	}
}

#endif

namespace Time{
	static std::chrono::milliseconds startTime;
	static std::chrono::milliseconds endTime;
	static unsigned int delta = 0;
	
	using namespace std::chrono;
	// In milliseconds
	static unsigned int getElapsedTime(){
		unsigned res = delta;
		delta = 0;
		return res;
	}
	static float resetDelta(){
		delta = 0;
	}
	static float startTimer(){
		startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}
	static float endTimer(){
		endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		delta += endTime.count() - startTime.count();
	}
}

#endif
