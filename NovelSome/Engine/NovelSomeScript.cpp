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
    
    std::wstring ParseArgument(CommandSettings& results)
    {
        unsigned int pos{ results.lastPos };
        std::wstring text = L"";
        bool Found{ false };
        bool insideQuotes{ false };
        
        while (!Found && pos+1 < results.line.length())
        {
            if (results.line[pos] == '"')
                insideQuotes = !insideQuotes;
            if (!(Found = (results.line[pos] == ' ' && !insideQuotes)))
            {
                text += results.line[pos];
                pos++;
            }
        }
        results.lastPos = pos + 1;
        
        return text;
    }
    wchar_t** ParseArguments(CommandSettings& results)
    {
        wchar_t** arguments{ nullptr };
        int parsedArguments{ 0 };
        
        while (results.lastPos+1 < results.line.length())
        {
            SkipSpaces(results);
            std::wstring argument = nss::ParseArgument(results);
            
            if (argument != L"with")
            {
                arguments = (wchar_t**)realloc(arguments, sizeof(wchar_t*)*(parsedArguments + 1));
                arguments[parsedArguments] = (wchar_t*)malloc(sizeof(wchar_t)*(argument.length() + 1));
                
                for (int i = 0; i <= argument.length(); i++)
                    arguments[parsedArguments][i] = argument[i];
                
                parsedArguments++;
            }
        }
        if (parsedArguments == 0)
            return nullptr;
        
        arguments = (wchar_t**)realloc(arguments, sizeof(wchar_t*)*(parsedArguments + 1));
        arguments[parsedArguments] = nullptr;
        
        return arguments;
    }
    std::wstring ArgumentAsString(CommandSettings& results)
    {
        std::wstring stringValue = nss::ParseUntil(results, '\0');
        return stringValue;
    }
    float ArgumentAsFloat(CommandSettings& results)
    {
        std::wstring stringValue = nss::ParseUntil(results, '\0');
        std::string parsingString = "";
        for (int i = 0; stringValue[i] != '\0'; i++)
            if (stringValue[i] == 46 || (stringValue[i] >= 48 && stringValue[i] <= 57))
                parsingString += (char)stringValue[i];
        
        return std::atof(parsingString.c_str());
    }
    int ArgumentAsInt(CommandSettings& results)
    {
        std::wstring stringValue = nss::ParseUntil(results, '\0');
        std::string parsingString = "";
        for (int i = 0; stringValue[i] != '\0'; i++)
            if (stringValue[i] >= 48 && stringValue[i] <= 57)
                parsingString += (char)stringValue[i];
        
        return std::atoi(parsingString.c_str());
    }
    /*std::wstring argument;
     while (command.lastPos+1 < command.line.length())
     {
     SkipSpaces(command);
     argument = nss::ParseArgument(command);
     std::cout << "ARGUMENT: '" << sf::String(argument).toAnsiString() << '\'' << std::endl;
     }*/
    
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
    
    int64_t power(int num, int by)
    {
        if (by == 0)
            return 1;
        
        int64_t res = num;
        for (int i = 1; i < by; i++)
            res *= num;
        return res;
    }
    
    //TODO: Documentation
    //TODO: Make this function working
    float ConvertToFloat(std::wstring str)
    {
        float value{ 0.f };
        int pointPos{ -1 };
        
        for (int i = 0; str[i] != '\0' && pointPos == -1; i++)
            if (str[i] == '.')
                pointPos = i;
        
        int startPos = (pointPos == -1) ? str.length()-1 : pointPos-1;
        for (int i = startPos; i >= 0; i--)
            if (str[i] >= 48 && str[i] <= 57)
            {
                if (startPos - i <= 36)
                {
                    std::cout << str[i] - 48 << " " << (startPos - i) << " " << power(10, (startPos - i)) << std::endl;
                    int64_t i64 = (str[i] - 48) * power(10, (startPos - i));
                    value += i64;
                    std::cout << value << std::endl;
                }
                else
                    std::cout << "Warning :: NSS :: ConvertToFloat :: Float can't hold more that 38 decimal digits." << std::endl;
            }
        
        //TODO: parse the after point
        
        return value;
    }
}
