//
//  NovelSomeScript.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef NovelSomeScript_hpp
#define NovelSomeScript_hpp

#include <iostream>
#include <string>

namespace nss
{
    struct CommandSettings
    {
        std::wstring line{ L"" };
        unsigned int lastPos{ 0 };
        bool keepTheLastPos{ true };
        int option1{ 0 };
        
        void Command(std::wstring line);
        void Step();
    };
    
    ///----------------------------------------------------------
    /// \brief Checks if the parsed string begins with command.
    ///
    /// Checks if the parsed string begins with command.
    /// Возвращает истину, если данная строка начинается с указанной команды, иначе - ложь.
    ///
    /// \param results Structure of type CommandSettings that contains specific settings and info.
    /// \param command The command to check for.
    ///
    /// \returns true if parsed string starts with specified command, false otherwise.
    ///----------------------------------------------------------
    bool Command(CommandSettings& results, const std::wstring& command);
    bool Command(const std::wstring& line, const std::wstring& command);
    
    //TODO: Documentation
    void SkipSpaces(CommandSettings& results);
    
    //TODO: Documentation
    std::wstring GetFromUntil(const std::wstring& line, unsigned int pos, const wchar_t until);
    
    //TODO: Documentation
    std::wstring ParseUntil(CommandSettings& results, const wchar_t until);
    
    //TODO: Documentation
    std::wstring ParseArgument(CommandSettings& results);
    wchar_t** ParseArguments(CommandSettings& results);
    std::wstring ArgumentAsString(CommandSettings& results);
    float ArgumentAsFloat(CommandSettings& results);
    int ArgumentAsInt(CommandSettings& results);
    
    //TODO: Documentation
    std::wstring GetFolderPath(std::wstring path);
    
    int64_t power(int num, int by);
    //TODO: Documentation
    float ConvertToFloat(std::wstring str);
}

#endif /* NovelSomeScript_hpp */
