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
#include <SFML/Config.hpp>
#ifdef SFML_SYSTEM_ANDROID
    #include <stdio.h>
    #include "ResourcePath.hpp"

    #include <jni.h>
    #include <android/native_activity.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
    #include <android/log.h>
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sfml-activity", __VA_ARGS__))
    #include <SFML/System/NativeActivity.hpp>
#endif

//MACROS
// _WIN32 - both 32-bit and 64-bit
// _WIN64
// __APPLE__
// __ANDROID__
// TARGET_OS_IPHONE
// TARGET_OS_MAC
// __linux__
// __unix__

#undef CreateDirectory
namespace ns
{
    namespace base
    {
        std::string utf8(const std::wstring& str);
        std::wstring utf16(const std::string& str);
        std::string utf8(const wchar_t& wchar);
        std::wstring utf16(const char& wchar);
        
        std::wstring GetCurrentWorkingDir();
        bool FileExists(const std::wstring& path);
        bool CreateDirectory(const std::wstring& path);
        
        bool DoesFileExistWithResolutionClass(const std::wstring& currentPath);
        
        std::wstring GetFolderPath(const std::wstring& path);
        std::wstring GetExtentionFromString(const std::wstring& filename);
        std::wstring GetStringWithNoExtention(const std::wstring& filename);
        
        size_t GetLengthWONewLinesAndSpaces(const std::wstring& string);
        
        std::wstring LowercaseTheString(const std::wstring& str);
        int atoi(const std::wstring& stringValue);
        int ConvertToInt(const std::string& stringValue);
        float atof(const std::wstring& stringValue);
        bool atob(const std::wstring& stringValue);
        
        int64_t power(int num, int by);
    }
}

#endif /* BaseFunctions_hpp */
