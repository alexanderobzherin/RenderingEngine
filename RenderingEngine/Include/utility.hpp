// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"

namespace rendering_engine
{
/**
 * @class Utility
 * @brief Provides static helper methods for file I/O and path management.
 *
 * The Utility class centralizes filesystem operations such as locating shader binaries,
 * listing files, and resolving build or application paths.
 *
 * All methods are static and thread-safe, designed for use throughout the engine.
 */
class Utility final
{
public:
    /**
     * @brief Initializes engine paths based on the executable location.
     *
     * Should be called once during startup to establish base directories for
     * application, build, and shader assets.
     *
     * @param argc Command-line argument count.
     * @param argv Command-line argument vector.
     */
	static void InitializePaths(int argc, char* argv[]);
	/**
	 * @brief Reads a binary shader file from disk.
	 * @param filename Path to the shader file.
	 * @return Vector containing the binary data.
	 */
    static std::vector<char> ReadShaderBinaryFile( std::string const & filename );
	/**
	 * @brief Returns a list of full file paths in the given directory.
	 * @param directory Directory path to search.
	 * @return Vector of file paths.
	 */
    static std::vector<std::string> GetListOfFilesInDirectory( std::string directory );
	/**
	 * @brief Returns the absolute path of the running application.
	 * @return Application path as boost::filesystem::path.
	 */
	static boost::filesystem::path GetApplicationPath();
	/**
	 * @brief Returns the build output directory path.
	 * @return Build path as boost::filesystem::path.
	 */
	static boost::filesystem::path GetBuildPath();
	/**
	 * @brief Returns the directory path containing compiled shader binaries.
	 * @return Shader binary path as boost::filesystem::path.
	 */
	static boost::filesystem::path GetShadersBinaryPath();
	/**
	 * @brief Returns a list of file names in a directory matching the specified extension.
	 * @param directory Directory path.
	 * @param extToSearch File extension to match (e.g., ".spv").
	 * @return Vector of matching file names.
	 */
	static std::vector<std::string> GetListOfFileNamesInDirectory(const char* directory, std::string extToSearch);

	private:
		Utility();
		Utility( const Utility& rhs );
		Utility& operator=( const Utility& rhs );

		static boost::filesystem::path FindPath( std::string fileOrFolderName, std::string searchingFrom = "../../" );

	private:
		static boost::filesystem::path sApplicationPath;
		static boost::filesystem::path const sDefaultShadersBinaryRelativePath;
		static boost::filesystem::path sBuildPath;
		static boost::filesystem::path sShadersBinaryPath;
		
};

template <typename T>
class Enum final
{
public:
	class Iterator final
	{
	public:
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using reference = T;
		using pointer = T*;
		using iterator_category = std::forward_iterator_tag;

		Iterator() = default;
		explicit Iterator(T value);
		Iterator(const Iterator&) = default;
		Iterator(Iterator&&) = default;
		Iterator& operator=(const Iterator&) = default;
		Iterator& operator=(Iterator&&) = default;
		~Iterator() = default;

		T operator*() const;
		Iterator& operator++();
		Iterator operator++(int);

		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;

	private:
		T mValue;
	};

	Iterator begin() const;
	Iterator end() const;
};

template <typename T>
inline Enum<T>::Iterator::Iterator(T value) :
	mValue(value)
{
}

template <typename T>
inline bool Enum<T>::Iterator::operator==(const Iterator& rhs) const
{
	return mValue == rhs.mValue;
}

template <typename T>
inline bool Enum<T>::Iterator::operator!=(const Iterator& rhs) const
{
	return mValue != rhs.mValue;
}

template <typename T>
inline T Enum<T>::Iterator::operator*() const
{
	if( mValue > T::End )
	{
		throw std::runtime_error("Cannot dereference end().");
	}

	return mValue;
}

template <typename T>
inline typename Enum<T>::Iterator& Enum<T>::Iterator::operator++()
{
	if( mValue <= T::End )
	{
		mValue = T(static_cast<int>(mValue) + 1);
	}
	return *this;
}

template <typename T>
inline typename Enum<T>::Iterator Enum<T>::Iterator::operator++(int)
{
	Iterator temp = *this;
	operator++();
	return temp;
}

template <typename T>
inline typename Enum<T>::Iterator Enum<T>::begin() const
{
	return Iterator(T::Begin);
}

template <typename T>
inline typename Enum<T>::Iterator Enum<T>::end() const
{
	return Iterator(T(static_cast<int>(T::End) + 1));
}
}