// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "logger.hpp"

#include "utility.hpp"

#include "boost/filesystem.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace rendering_engine
{

namespace
{
std::string LogLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug:   return "DEBUG";
    case LogLevel::Info:    return "INFO";
    case LogLevel::Warning: return "WARNING";
    case LogLevel::Error:   return "ERROR";
    default:                return "UNKNOWN";
    }
}
}

Logger& Logger::Get()
{
    static Logger instance;
    return instance;
}

void Logger::Initialize(const std::string& appName)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mFile.is_open())
        return;

    auto logFolderPath = Utility::GetLogsFolderPath();
    if (!boost::filesystem::exists(logFolderPath))
    {
        boost::filesystem::create_directory(logFolderPath);
    }

    // System clock for human-readable timestamp
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &nowTimeT);
#else
    localtime_r(&nowTimeT, &localTime);
#endif

    std::ostringstream filename;
    filename << logFolderPath.string() << "/"
        << appName << "_"
        << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S")
        << ".log";

    mFile.open(filename.str(), std::ios::out | std::ios::trunc);

    mCurrentLevel = ReadConfiguredLogLevel();

    if (mFile.is_open())
    {
        mFile << "===== Engine Log Started =====" << std::endl;
        mFile.flush();
    }
}

void Logger::Shutdown()
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mFile.is_open())
    {
        mFile << "===== Engine Log Shutdown =====" << std::endl;
        mFile.flush();
        mFile.close();
    }
}

LogLevel Logger::ReadConfiguredLogLevel()
{
    LogLevel configuredLevel = LogLevel::Info;

    const std::string confLogLevelStr = Utility::ReadConfigFile().logLevel;
    if (confLogLevelStr == std::string{ "Debug" })
    {
        configuredLevel = LogLevel::Debug;
    }
    if (confLogLevelStr == std::string{ "Warning" })
    {
        configuredLevel = LogLevel::Warning;
    }
    if (confLogLevelStr == std::string{ "Error" })
    {
        configuredLevel = LogLevel::Error;
    }

    return configuredLevel;
}

void Logger::SetLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mCurrentLevel = level;
}

void Logger::Log(LogLevel level,
    const std::string& message,
    const char* file,
    int line)
{
    if (level < mCurrentLevel)
        return;

    if (!mFile.is_open())
        return;

    // Capture time outside lock for minimal contention
    auto nowSystem = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(nowSystem);
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        nowSystem.time_since_epoch()) % 1000;

    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &nowTimeT);
#else
    localtime_r(&nowTimeT, &localTime);
#endif

    std::ostringstream timestampStream;
    timestampStream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << nowMs.count();

    const auto threadId = std::this_thread::get_id();

    std::lock_guard<std::mutex> lock(mMutex);

    mFile << "[" << timestampStream.str() << "] "
        << "[" << LogLevelToString(level) << "] "
        << "[Thread " << threadId << "] "
        << message
        << " (" << file << ":" << line << ")"
        << std::endl;

    mFile.flush();
}

} // namespace rendering_engine