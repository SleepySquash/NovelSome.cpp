//
//  Base.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/06/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
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
        std::wstring GetCurrentWorkingDir();
        
        //TODO: Documentation
        std::wstring GetFolderPath(std::wstring path);
        std::wstring GetExtentionFromString(std::wstring filename);
        std::wstring GetStringWithNoExtention(std::wstring filename);
        
        bool FileExists(std::wstring path);
        std::wstring LowercaseTheString(const std::wstring& str);

        int64_t power(int num, int by);
        float ConvertToFloat(std::wstring str);

        /*Объявляем вспомогательные функции*/
        char* InputString();
        int GetLength(const char*);
    }
}

#endif /* BaseFunctions_hpp */
