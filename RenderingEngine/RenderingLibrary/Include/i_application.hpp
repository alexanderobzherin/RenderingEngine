// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_renderer.hpp"
#include <string>

namespace rendering_engine
{

/**
 * @struct ScreenSettings
 * @brief Describes window and display configuration for the application.
 *
 * This structure holds parameters defining how the application window
 * or rendering surface is initialized, including its name, resolution,
 * and display mode.
 */
struct ScreenSettings
{
    /** @brief The window or application name. */
    std::string name;
    /** @brief Whether the application runs in fullscreen mode. */
    bool isFullScreen;
    /** @brief Screen or window width in pixels. */
    unsigned int width;
    /** @brief Screen or window height in pixels. */
    unsigned int height;
};

/**
 * @struct FrameMetrics
 * @brief Stores CPU timing statistics for a rendered frame.
 *
 * Contains frame duration, update time, draw time,
 * and FPS values (raw and smoothed).
 *
 * All time values are in milliseconds.
 */
struct FrameMetrics
{
    /** Total frame duration (ms). */
    float frameDurationMs = 0.0f;
    /** Time spent in Update() (ms). */
    float updateTimeMs = 0.0f;
    /** Time spent in Draw() (ms). */
    float drawTimeMs = 0.0f;
    /** Instantaneous FPS (1000 / frameDurationMs). */
    float fpsRaw = 0.0f;
    /** Smoothed FPS value. */
    float fpsSmoothed = 0.0f;
};

/**
 * @class IApplication
 * @brief Defines a generic application interface for rendering-based programs.
 *
 * Provides lifecycle control for initialization, main loop, update, rendering,
 * and shutdown. All specific applications built on top of the Rendering Engine
 * should implement this interface.
 *
 * @see IRenderer
 */
class IApplication
{
public:
    /** @brief Initializes the application and its subsystems. */
    virtual void Initialize() = 0;
    /** @brief Runs the main application loop. */
    virtual void Run() = 0;
    /**
     * @brief Updates the application state.
     * @param deltaTime Time elapsed since the previous frame, in milliseconds.
     */
    virtual void Update(float deltaTime) = 0;
    /** @brief Executes the rendering logic for the current frame. */
    virtual void Draw() = 0;
    /** @brief Performs cleanup and shuts down the application. */
    virtual void Shutdown() = 0;
    /** @brief Virtual destructor for safe polymorphic deletion. */
    virtual ~IApplication() = default;
    /**
     * @brief Retrieves the current screen or window settings.
     * @return A structure describing screen configuration parameters.
     */
    virtual ScreenSettings GetScreenSettings() const = 0;
    /**
     * @brief Returns performance metrics of the last processed frame.
     *
     * @return FrameMetrics structure containing timing and FPS data.
     */
    virtual FrameMetrics GetFrameMetrics() const = 0;
};
} //rendering_engine