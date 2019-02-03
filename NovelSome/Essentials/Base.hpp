//
//  Base.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/06/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef BaseFunctions_hpp
#define BaseFunctions_hpp

#include <iostream>

//FileExists
#include <codecvt>
#include <sys/stat.h>

//MACROS
// _WIN32 - both 32-bit and 64-bit
// _WIN64
// __APPLE__
// TARGET_OS_IPHONE
// TARGET_OS_MAC
// __linux__
// __unix__


namespace ns
{
    namespace base
    {
        std::string utf8(const std::wstring& str);
        std::wstring utf16(const std::string& str);
        std::string utf8(const wchar_t& wchar);
        std::wstring utf16(const char& wchar);
        
        std::wstring GetCurrentWorkingDir();
        bool FileExists(std::wstring path);
        bool DoesFileExistWithResolutionClass(const std::wstring& currentPath);
        bool CreateDirectory(const std::wstring& path);
        
        std::wstring GetFolderPath(std::wstring path);
        std::wstring GetExtentionFromString(std::wstring filename);
        std::wstring GetStringWithNoExtention(std::wstring filename);
        std::wstring GetStringWithNoExtention(std::wstring filename, const std::wstring& extention);
        
        size_t GetLengthWONewLinesAndSpaces(const std::wstring& string);
        
        std::wstring LowercaseTheString(const std::wstring& str);
        int ConvertToInt(const std::wstring& stringValue);
        int ConvertToInt(const std::string& stringValue);
        float ConvertToFloat(const std::wstring& stringValue);
        bool ConvertToBool(const std::wstring& stringValue);
        
        int64_t power(int num, int by);
    }
}

#endif /* BaseFunctions_hpp */
