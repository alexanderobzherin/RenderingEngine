#pragma once

#include <chrono>
namespace rendering_engine
{
	
class AppTime;

class AppClock
{
public:
	AppClock();
	AppClock(AppClock const&) = default;
	AppClock& operator=(AppClock const&) = default;
	AppClock(AppClock &&) = default;
	AppClock& operator=(AppClock &&) = default;
	~AppClock() = default;

	const std::chrono::high_resolution_clock::time_point& StartTime() const;
	const std::chrono::high_resolution_clock::time_point& CurrentTime() const;
	const std::chrono::high_resolution_clock::time_point& LastTime() const;

	void Reset();
	void UpdateAppTime(AppTime& appTime);

private:
	std::chrono::high_resolution_clock::time_point mStartTime;
	std::chrono::high_resolution_clock::time_point mCurrentTime;
	std::chrono::high_resolution_clock::time_point mLastTime;
};

} //rendering_engine