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

#include "ResourcePath.hpp"
//FileExists
#include <codecvt>
#include <sys/stat.h>
#include <SFML/Config.hpp>
#ifdef SFML_SYSTEM_ANDROID
    #include <stdio.h>

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

using std::cout;
using std::endl;

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
        std::wstring GetFilenameWOProhibited(const std::wstring& filename);
        
        size_t GetLengthWONewLinesAndSpaces(const std::wstring& string);
        
        std::wstring LowercaseTheString(const std::wstring& str);
        int ConvertToInt(const std::string& stringValue);
        int atoi(const std::wstring& stringValue);
        long atol(const std::wstring& stringValue);
        float atof(const std::wstring& stringValue);
        bool atob(const std::wstring& stringValue);
        
        double atan(double x);
        
        //////////////////////////////////////////////////////////////////////////////
        /// This code below belongs to https://stackoverflow.com/users/592323/leemes
        //////////////////////////////////////////////////////////////////////////////
        template<typename It> class Range
        {
            It b, e;
        public:
            Range(It b, It e) : b(b), e(e) {}
            It begin() const { return b; }
            It end() const { return e; }
        };
        template<typename ORange, typename OIt = decltype(std::begin(std::declval<ORange>())), typename It = std::reverse_iterator<OIt>>
        Range<It> reverse(ORange && originalRange) { return Range<It>(It(std::end(originalRange)), It(std::begin(originalRange))); }
    }
}

#endif /* BaseFunctions_hpp */
