//
//  NovelSomeScript.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "NovelSomeScript.hpp"

namespace nss
{
    void CommandSettings::Command(std::wstring line)
    {
        this->line = line;
        lastPos = 0;
    }
    
    void CommandSettings::Step()
    {
        lastPos++;
    }
    
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
    bool Command(CommandSettings& results, const std::wstring& command)
    {
		std::wcout << L"Debug: '" << results.line << "'    '" << command << "'" << std::endl;
        bool ReallyFound{ false };
        
        SkipSpaces(results);
        if (command.length() <= results.line.length())
        {
            bool Found{ true };
            int i = results.keepTheLastPos ? results.lastPos : 0;
            for (int j = 0; Found && j < command.length(); i++, j++)
                Found = (results.line[i] == command[j]);
            
            if (Found)
                results.lastPos = i;
            
            ReallyFound = Found;
        }
        
        return ReallyFound;
    }
    bool Command(const std::wstring& line, const std::wstring& command)
    {
        bool ReallyFound{ false };
        
        if (command.length() <= line.length())
        {
            bool Found{ true };
            for (int i = 0; Found && i < command.length(); i++)
                Found = (line[i] == command[i]);
            
            ReallyFound = Found;
        }
        
        return ReallyFound;
    }
    
    //TODO: Documentation
    void SkipSpaces(CommandSettings& results)
    {
        while (results.lastPos < results.line.length() && (results.line[results.lastPos] == L' ' || results.line[results.lastPos] == L'\t'))
            results.lastPos++;
    }
    
    //TODO: Documentation
    std::wstring ParseUntil(CommandSettings& results, const wchar_t until)
    {
        unsigned int pos{ results.lastPos };
        std::wstring text = L"";
        bool Found{ false };
        
        while (!Found && pos < results.line.length())
        {
            if (!(Found = (results.line[pos] == until)))
            {
                text += results.line[pos];
                pos++;
            }
        }
        results.lastPos = pos + 1;
        
        return text;
    }
    
    //TODO: Documentation
    std::wstring GetFromUntil(const std::wstring& line, unsigned int pos, const wchar_t until)
    {
        std::wstring text = L"";
        bool Found{ false };
        
        while (!Found && pos < line.length())
        {
            if (!(Found = (line[pos] == until)))
            {
                text += line[pos];
                pos++;
            }
        }
        
        return text;
    }
    
    //TODO: Documentation
    std::wstring GetFolderPath(std::wstring path)
    {
        std::wstring folder = L"";
        
        int pos{ -1 };
        if (path.length() != 0)
            for (unsigned int i = path.length() - 1; i >= 0 && pos == -1; i--)
                if (path[i] == '/' || path[i] == '\\')
                    pos = i;
        
        for (int i = 0; i <= pos; i++)
            folder += path[i];
        
        return folder;
    }
}
