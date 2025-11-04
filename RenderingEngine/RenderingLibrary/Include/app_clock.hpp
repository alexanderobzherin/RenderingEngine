// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

/**
 * @file app_clock.hpp
 * @brief High-resolution clock for updating application time.
 *
 * @details
 * AppClock measures precise frame timing using `std::chrono::high_resolution_clock`
 * and updates an associated AppTime instance each frame. It tracks the start,
 * last, and current timestamps, enabling the engine to compute total and
 * delta time values.
 *
 * @see rendering_engine::AppTime
 */
#pragma once

#include <chrono>
namespace rendering_engine
{
	
class AppTime;
/**
 * @class AppClock
 * @brief Provides high-resolution timing for frame updates.
 *
 * AppClock tracks time since the application started and between frames.
 * Each call to UpdateAppTime() updates an AppTime instance with new total
 * and elapsed durations.
 *
 * Typical usage:
 * @code
 * AppClock clock;
 * AppTime time;
 * clock.Reset();
 *
 * while (running)
 * {
 *     clock.UpdateAppTime(time);
 *     float delta = time.ElapsedAppTimeMilliseconds();
 *     // Update logic...
 * }
 * @endcode
 *
 * @see rendering_engine::AppTime
 */
class AppClock
{
public:
	/// Constructs a new clock initialized with the current time.
	AppClock();
	AppClock(AppClock const&) = default;
	AppClock& operator=(AppClock const&) = default;
	AppClock(AppClock &&) = default;
	AppClock& operator=(AppClock &&) = default;
	~AppClock() = default;
	/// Returns the timestamp when the clock was started.
	const std::chrono::high_resolution_clock::time_point& StartTime() const;
	/// Returns the most recent recorded time (current frame).
	const std::chrono::high_resolution_clock::time_point& CurrentTime() const;
	/// Returns the time recorded at the previous frame.
	const std::chrono::high_resolution_clock::time_point& LastTime() const;
	/// Resets the clock to the current system time.
	void Reset();
	/**
	 * @brief Updates the given AppTime instance with elapsed and total durations.
	 *
	 * Calculates the time difference between the last and current frame,
	 * updates internal state, and synchronizes AppTime accordingly.
	 *
	 * @param appTime Reference to the AppTime instance to update.
	 */
	void UpdateAppTime(AppTime& appTime);

private:
	std::chrono::high_resolution_clock::time_point mStartTime;
	std::chrono::high_resolution_clock::time_point mCurrentTime;
	std::chrono::high_resolution_clock::time_point mLastTime;
};

} //rendering_engine