// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

/**
 * @file logger.hpp
 * @brief Engine-wide logging system for runtime diagnostics and performance tracking.
 *
 * The Logger class provides a lightweight, thread-safe logging facility
 * for the Rendering Engine. A dedicated log file is created per application run.
 *
 * Log messages include:
 *  - Timestamp (human-readable)
 *  - Log level (Debug, Info, Warning, Error)
 *  - Thread ID
 *  - Source file and line number
 *
 * The logger supports runtime-configurable log levels via application configuration.
 *
 * Usage:
 * @code
 * LOG_INFO("Renderer initialized");
 * LOG_ERROR("Failed to load texture");
 * @endcode
 */

#pragma once

#include "rendering_engine_export.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include  <mutex>


#define LOG_DEBUG(msg)  rendering_engine::Logger::Get().Log(LogLevel::Debug,  msg, __FILE__, __LINE__)
#define LOG_INFO(msg)  rendering_engine::Logger::Get().Log(LogLevel::Info,  msg, __FILE__, __LINE__)
#define LOG_WARNING(msg)  rendering_engine::Logger::Get().Log(LogLevel::Warning,  msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) rendering_engine::Logger::Get().Log(LogLevel::Error, msg, __FILE__, __LINE__)

namespace rendering_engine
{
/**
 * @enum LogLevel
 * @brief Defines severity levels for log messages.
 *
 * Levels are ordered by increasing severity:
 * Debug < Info < Warning < Error
 *
 * Log filtering is performed by comparing message level
 * against the configured runtime level.
 */
enum class LogLevel
{
    Debug,   ///< Detailed diagnostic information
    Info,    ///< General informational messages
    Warning, ///< Non-critical issues or unexpected states
    Error    ///< Critical errors affecting functionality
};
/**
 * @class Logger
 * @brief Singleton engine-wide logging system.
 *
 * Responsibilities:
 *  - Create log file per application run
 *  - Write formatted log entries
 *  - Apply runtime log-level filtering
 *  - Ensure thread-safe file access
 *
 * The logger must be initialized during application startup
 * and shutdown during application termination.
 */
class RE_API Logger
{
public:
    /**
     * @brief Returns singleton logger instance.
     */
    static Logger& Get();
    /**
     * @brief Initializes logging system and creates a new log file.
     * @param appName Name of the application (used in log filename).
     */
    void Initialize(const std::string& appName);
    /**
     * @brief Finalizes logging system and closes the log file.
     */
    void Shutdown();
    /**
     * @brief Writes a formatted log message.
     *
     * @param level Log severity level.
     * @param message Log message string.
     * @param file Source file name.
     * @param line Source line number.
     */
    void Log(LogLevel level,
        const std::string& message,
        const char* file,
        int line);

private:
    /**
     * @brief Reads configured log level from application config file.
     * @return LogLevel parsed from configuration.
     */
    LogLevel ReadConfiguredLogLevel();
    /**
     * @brief Sets current runtime log level.
     * @param level Desired log level.
     */
    void SetLogLevel(LogLevel level);

private:
    std::ofstream mFile;
    std::mutex mMutex;
    LogLevel mCurrentLevel;
};
} // namespace rendering_engine
