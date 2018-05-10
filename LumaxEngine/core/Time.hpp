
#ifndef TIME_HPP
#define TIME_HPP

#include "../RAL/buildDesc.hpp"

#ifdef _WINDOWS

#include "../math/types.hpp"
#include <windows.h>

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
#endif
