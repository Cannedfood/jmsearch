#pragma once

#include <chrono>

class Timer {
	std::chrono::high_resolution_clock::time_point mStart;

public:
	Timer() { start(); }
	~Timer() {}

	auto now()   { return std::chrono::high_resolution_clock::now(); }
	void start() { mStart = now(); }

	size_t milliseconds() { return std::chrono::duration_cast<std::chrono::milliseconds>(now() - mStart).count(); }
	size_t microseconds() { return std::chrono::duration_cast<std::chrono::microseconds>(now() - mStart).count(); }
};
