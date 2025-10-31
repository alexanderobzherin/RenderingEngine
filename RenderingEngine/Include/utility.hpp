#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"

namespace rendering_engine
{
class Utility final
{
    public:
	static void InitializePaths(int argc, char* argv[]);
    static std::vector<char> ReadShaderBinaryFile( std::string const & filename );


    static std::vector<std::string> GetListOfFilesInDirectory( std::string directory );
	static boost::filesystem::path GetApplicationPath();
	static boost::filesystem::path GetBuildPath();
	static boost::filesystem::path GetShadersBinaryPath();
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