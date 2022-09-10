#pragma once

#include <chrono>

namespace rendering_engine
{

class AppTime
{
public:
	AppTime();

	const std::chrono::high_resolution_clock::time_point& CurrentTime() const;
	void SetCurrentTime(const std::chrono::high_resolution_clock::time_point& currentTime);

	const std::chrono::milliseconds& TotalAppTime() const;
	void SetTotalAppTime(const std::chrono::milliseconds& totalAppTime);

	const float ElapsedAppTimeMilliseconds() const;

	const std::chrono::milliseconds& ElapsedAppTime() const;
	void SetElapsedAppTime(const std::chrono::milliseconds& elapsedAppTime);

private:
	std::chrono::high_resolution_clock::time_point mCurrentTime;
	std::chrono::milliseconds mTotalAppTime;
	std::chrono::milliseconds mElapsedAppTime;
};

} //rendering_engine