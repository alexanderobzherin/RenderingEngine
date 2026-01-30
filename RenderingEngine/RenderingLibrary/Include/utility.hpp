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

#include "rendering_engine_export.hpp"

namespace rendering_engine
{
/**
 * @struct AppConfig
 * @brief Basic application settings loaded from a configuration file.
 *
 * This structure represents a minimal configuration schema for applications
 * built using the Rendering Engine. Values are typically read from
 * `Config/app_config.json` at startup and applied during window creation.
 */
struct AppConfig
{
	/** @brief Name of the application */
	std::string appName;
	/** @brief Whether the application should start in full-screen mode. */
	bool isFullScreen = false;
	/** @brief Desired window width in pixels (ignored in full-screen mode). */
	float screenWidth = 800.0f;
	/** @brief Desired window height in pixels (ignored in full-screen mode). */
	float screenHeight = 600.0f;

	std::vector<std::string> textScripts{"Latin"};
	std::vector<int> fontSizePreload{10};
};

/**
 * @struct PackEntry
 * @brief Metadata describing one file stored inside a packed asset archive.
 */
struct PackEntry
{
	size_t offset;
	size_t size;
};

using PackEntries = std::unordered_map<std::string, PackEntry>;

/**
 * @class Utility
 * @brief Provides static helper methods for file I/O and path management.
 *
 * The Utility class centralizes filesystem operations such as locating shader binaries,
 * listing files, and resolving build or application paths.
 *
 * All methods are static and thread-safe, designed for use throughout the engine.
 */
class RE_API Utility final
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
	 * @brief Reads application settings from the JSON config file.
	 *
	 * Missing or invalid fields fall back to default AppConfig values.
	 *
	 * @return Populated AppConfig structure.
	 */
	static AppConfig ReadConfigFile();
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

	/// @brief Resolves project root folder (handles Release/Debug/Binaries layouts).
	static boost::filesystem::path ResolveProjectRoot();

	/// @brief Returns absolute path to Content.
	static boost::filesystem::path GetContentFolderPath();

	/// @brief Returns absolute path to Content/Textures.
	static boost::filesystem::path GetTextureFolderPath();

	/// @brief Returns absolute path to Content/Models.
	static boost::filesystem::path GetModelsFolderPath();

	/// @brief Returns absolute path to Content/Fonts.
	static boost::filesystem::path GetFontsFolderPath();

	/// @brief Returns absolute path to Content/Shaders.
	static boost::filesystem::path GetShadersFolderPath();

	/// @brief Returns absolute path to Config/app_config.json.
	static boost::filesystem::path GetConfigFilePath();

	/**
     * @brief Checks whether packed assets (Pack.bin / Pack.json) exist.
     */
	static bool IsPackageProvided();

	/**
     * @brief Returns the manifest of packed files.
     */
	static const PackEntries& GetPackEntries();

	/**
     * @brief Reads raw bytes of a file stored inside Pack.bin.
     *
     * @param entryPath Virtual path inside the pack (e.g. "Textures/my.png").
     * @return Decoded file bytes, or empty vector on failure.
     */
	static std::vector<uint8_t> ReadPackedFile(const std::string& entryPath);

	private:
		Utility();
		Utility( const Utility& rhs );
		Utility& operator=( const Utility& rhs );

		static boost::filesystem::path FindPath( std::string fileOrFolderName, std::string searchingFrom = "../../" );

	public:
		static boost::filesystem::path sApplicationPath;
		static boost::filesystem::path const sDefaultShadersBinaryRelativePath;
		static boost::filesystem::path sBuildPath;
		static boost::filesystem::path sShadersBinaryPath;
		static boost::filesystem::path const sContentRelativePathFolder;
		static boost::filesystem::path const sContentPackageFilePath;
		static boost::filesystem::path const sContentPackEntriesFilePath;
		static boost::filesystem::path const sTextureRelativePathFolder;
		static boost::filesystem::path const sModelsRelativePathFolder;
		static boost::filesystem::path const sFontsRelativePathFolder;
		static boost::filesystem::path const sShadersRelativePathFolder;
		static boost::filesystem::path const sAppConfigFilePath;
		
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