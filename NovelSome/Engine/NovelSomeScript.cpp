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
        //this->line = ns::base::LowercaseTheString(line);
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
        std::wstring commandLine = results.lowercaseCommand ? ns::base::LowercaseTheString(results.line) : results.line;
        if (command.length() <= commandLine.length())
        {
            bool Found{ true };
            int i = results.keepTheLastPos ? results.lastPos : 0;
            for (int j = 0; Found && j < command.length(); i++, j++)
                Found = (commandLine[i] == command[j]);
            
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
                if (results.line[pos] != 13)
                    text += results.line[pos];
                pos++;
            }
        }
        results.lastPos = pos + 1;
        
        return text;
    }
    
    //TODO: Documentation
    std::wstring ParseWhile(CommandSettings& results, const wchar_t until)
    {
        unsigned int pos{ results.lastPos };
        std::wstring text = L"";
        bool Found{ true };
        
        while (Found && pos < results.line.length())
        {
            if ((Found = (results.line[pos] == until)))
            {
                if (results.line[pos] != 13)
                    text += results.line[pos];
                pos++;
            }
        }
        
        return text;
    }
    
    //TODO: Documentation
    std::wstring ParseAsQuoteString(CommandSettings& results)
    {
        SkipSpaces(results);
        
        unsigned int pos{ results.lastPos };
        std::wstring text = L"";
        bool QuotesInline{ false }, Found{ false };
        
        while (!Found && pos < results.line.length())
        {
            Found = (results.line[pos] == '"');
            
            if (!Found)
                text += results.line[pos];
            
            if (!QuotesInline && Found)
            {
                QuotesInline = true;
                Found = false;
            }
            
            pos++;
        }
        if (Found)
        {
            results.lastPos = pos;
            return text;
        }
        else
            return L"";
    }
    
    int ParseAsInt(CommandSettings& results)
    {
        //std::wstring stringValue = nss::ParseUntil(results, ' ');
        return ns::base::ConvertToInt(nss::ParseUntil(results, ' '));
    }
    float ParseAsFloat(CommandSettings& results)
    {
        //std::wstring stringValue = nss::ParseUntil(results, ' ');
        return ns::base::ConvertToFloat(nss::ParseUntil(results, ' '));
    }
    
    std::wstring ParseArgument(CommandSettings& results)
    {
        unsigned int pos{ results.lastPos };
        std::wstring text = L"";
        bool Found{ false };
        bool insideQuotes{ false };
        
        while (!Found && pos < results.line.length())
        {
            if (results.line[pos] == '"')
                insideQuotes = !insideQuotes;
            if (!(Found = (results.line[pos] == ' ' && !insideQuotes)))
            {
                if (results.line[pos] != 13 && results.line[pos] != ',')
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
            
            if (argument != L"with" && argument != L"as")
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
        std::wstring parsedUntil = nss::ParseUntil(results, '\0');
        std::wstring stringValue = ns::base::LowercaseTheString(parsedUntil);
        return stringValue;
    }
    std::wstring ArgumentAsStringWOLowerCase(CommandSettings& results)
    {
        std::wstring stringValue = nss::ParseUntil(results, '\0');
        return stringValue;
    }
    bool ArgumentAsBool(CommandSettings& results)
    {
        std::wstring stringValue = nss::ArgumentAsString(results);
        if (stringValue == L"true" || stringValue == L"1")
            return true;
        else
            return false;
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
    
    
    
    void SetStringWithLineBreaks(sf::Text& text, const std::wstring& line, const unsigned int width)
    {
        text.setString(line);
        if (text.getLocalBounds().width >= width)
        {
            std::wstring finalLine = L"";
            std::wstring currentLine = L"";
            for (int i = 0; i < line.length(); i++)
            {
                currentLine += line[i];
                text.setString(currentLine);
                if (text.getLocalBounds().width >= width)
                {
                    bool spaceFound{ false };
                    for (int j = currentLine.length() - 1; j >= 0 && !spaceFound; j--)
                    {
                        if ((spaceFound = (currentLine[j] == L' ')))
                        {
                            std::wstring toFinalLine;
                            for (int k = 0; k < j; k++)
                                toFinalLine += currentLine[k];
                            toFinalLine += L'\n';
                            finalLine += toFinalLine;
                            
                            std::wstring newCurrentLine = L"";
                            for (int k = j + 1; k < currentLine.length(); k++)
                                newCurrentLine += currentLine[k];
                            
                            currentLine = newCurrentLine;
                        }
                    }
                    if (!spaceFound)
                    {
                        currentLine[currentLine.length() - 1] = L'\n';
                        finalLine += currentLine;
                        currentLine = L"";
                        
                        currentLine += line[i];
                    }
                }
            }
            finalLine += currentLine;
            text.setString(finalLine);
        }
    }
    void SetStringWithLineBreaksWOSpaceFinding(sf::Text& text, const std::wstring& line, const unsigned int width)
    {
        text.setString(line);
        if (text.getLocalBounds().width >= width)
        {
            std::wstring finalLine = L"";
            std::wstring currentLine = L"";
            for (int i = 0; i < line.length(); i++)
            {
                currentLine += line[i];
                text.setString(currentLine);
                if (text.getLocalBounds().width >= width)
                {
                    currentLine[currentLine.length() - 1] = L'\n';
                    finalLine += currentLine;
                    currentLine = L"";
                    
                    currentLine += line[i];
                }
            }
            text.setString(finalLine);
        }
    }
}
