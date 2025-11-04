// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

/**
 * @file app_time.hpp
 * @brief Provides time tracking for the application runtime.
 *
 * @details
 * AppTime measures and stores total and per-frame elapsed time using
 * `std::chrono::high_resolution_clock`. It is typically used by the main
 * game loop to calculate delta times and track total uptime.
 *
 * Example:
 * @code
 * AppTime time;
 * time.SetCurrentTime(std::chrono::high_resolution_clock::now());
 * float delta = time.ElapsedAppTimeMilliseconds();
 * @endcode
 */
#pragma once

#include <chrono>

namespace rendering_engine
{
/**
 * @class AppTime
 * @brief Manages current, total, and elapsed time for the application.
 *
 * Stores high-resolution timestamps and durations for frame updates.
 * Provides both millisecond and chrono-based accessors.
 */
class AppTime
{
public:
	/// Constructs a new AppTime instance with zeroed timers.
	AppTime();
	/// Returns the current high-resolution time point.
	const std::chrono::high_resolution_clock::time_point& CurrentTime() const;
	/// Sets the current high-resolution time point
	void SetCurrentTime(const std::chrono::high_resolution_clock::time_point& currentTime);
	/// Returns total accumulated application time since start.
	const std::chrono::milliseconds& TotalAppTime() const;
	/// Returns total accumulated time in milliseconds (float).
	const float TotalAppTimeMilliseconds() const;
	/// Sets total accumulated application time.
	void SetTotalAppTime(const std::chrono::milliseconds& totalAppTime);
	/// Returns elapsed time since the last frame in milliseconds (float).
	const float ElapsedAppTimeMilliseconds() const;
	/// Returns elapsed time since the last frame.
	const std::chrono::milliseconds& ElapsedAppTime() const;
	/// Sets elapsed time since the last frame.
	void SetElapsedAppTime(const std::chrono::milliseconds& elapsedAppTime);

private:
	std::chrono::high_resolution_clock::time_point mCurrentTime;
	std::chrono::milliseconds mTotalAppTime;
	std::chrono::milliseconds mElapsedAppTime;
};

} //rendering_engine