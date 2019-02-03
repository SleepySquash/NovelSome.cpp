//
//  NovelSomeScript.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
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
        
        int lastPos{ 0 };
        while (lastPos < line.length() && (line[lastPos] == L' ' || line[lastPos] == L'\t')) ++lastPos;
        std::wstring commandLine = ns::base::LowercaseTheString(line);
        if (command.length() <= commandLine.length())
        {
            bool Found{ true };
            int i = lastPos;
            for (int j = 0; Found && j < command.length(); i++, j++)
                Found = (commandLine[i] == command[j]);
            ReallyFound = Found;
        }
        
        return ReallyFound;
    }
    bool Command(const std::string& line, const std::string& command)
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
    bool ContainsUsefulInformation(CommandSettings& results)
    {
        for (int i = results.lastPos; i < results.line.length(); i++)
            if (results.line[i] != L' ' && results.line[i] != L'\t' && results.line[i] != L'\0' && results.line[i] != L'\n' && results.line[i] != 13)
                return true;
        return false;
    }
    bool ContainsUsefulInformation(const std::wstring& wstr)
    {
        for (int i = 0; i < wstr.length(); i++)
            if (wstr[i] != L' ' && wstr[i] != L'\t' && wstr[i] != L'\0' && wstr[i] != L'\n' && wstr[i] != 13)
                return true;
        return false;
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
    std::string ParseUntil(std::string line, const char until, unsigned int from)
    {
        unsigned int pos{ from };
        std::string text = "";
        bool Found{ false };
        
        while (!Found && pos < line.length())
        {
            if (!(Found = (line[pos] == until)))
            {
                if (line[pos] != 13)
                    text += line[pos];
                pos++;
            }
        }
        
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
    std::wstring ParseAsMaybeQuoteString(CommandSettings& results)
    {
        if (results.line[results.lastPos] == L'"')
            return nss::ParseAsQuoteString(results);
        else
            return nss::ParseUntil(results, L' ');
    }
    std::wstring ParseAsMaybeQuoteStringFull(CommandSettings& results)
    {
        nss::SkipSpaces(results);
        if (results.line[results.lastPos] == L'"')
            return nss::ParseAsQuoteString(results);
        else
            return nss::ParseUntil(results, L'\0');
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
    std::wstring ParseAsString(CommandSettings& results)
    {
        std::wstring parsedUntil = nss::ParseUntil(results, '\0');
        std::wstring stringValue = ns::base::LowercaseTheString(parsedUntil);
        return stringValue;
    }
    std::wstring ParseAsStringWOLowercase(CommandSettings& results)
    {
        std::wstring parsedUntil = nss::ParseUntil(results, '\0');
        return parsedUntil;
    }
    bool ParseAsBool(CommandSettings& results)
    {
        std::wstring stringValue = nss::ParseAsString(results);
        if (stringValue == L"true" || stringValue == L"1")
            return true;
        else
            return false;
    }
    
    
    sf::Color ParseColor(CommandSettings& results)
    {
        nss::SkipSpaces(results);
        std::wstring color1 = nss::ParseUntil(results, ' ');
        if (color1.length() != 0)
        {
            nss::SkipSpaces(results);
            std::wstring color2 = nss::ParseUntil(results, ' ');
            if (color2.length() != 0)
            {
                nss::SkipSpaces(results);
                std::wstring color3 = nss::ParseUntil(results, ' ');
                if (color3.length() != 0)
                {
                    int rColor = ns::base::ConvertToInt(color1);
                    int gColor = ns::base::ConvertToInt(color2);
                    int bColor = ns::base::ConvertToInt(color3);
                    return sf::Color(rColor, gColor, bColor, 0);
                }
            }
            else
            {
                std::wstring guessColor = ns::base::LowercaseTheString(color1);
                if (guessColor == L"while")
                    return sf::Color(255, 255, 255, 0);
                else if (guessColor == L"black")
                    return sf::Color(0, 0, 0, 0);
                else if (guessColor == L"red")
                    return sf::Color(sf::Color::Red.r, sf::Color::Red.g, sf::Color::Red.b, 0);
                else if (guessColor == L"magenta")
                    return sf::Color(sf::Color::Magenta.r, sf::Color::Magenta.g, sf::Color::Magenta.b, 0);
                else if (guessColor == L"cyan")
                    return sf::Color(sf::Color::Cyan.r, sf::Color::Cyan.g, sf::Color::Cyan.b, 0);
                else if (guessColor == L"yellow")
                    return sf::Color(sf::Color::Yellow.r, sf::Color::Yellow.g, sf::Color::Yellow.b, 0);
            }
        }
        
        return sf::Color(0, 0, 0, 255);
    }
    int ParseAlpha(CommandSettings& results)
    {
        nss::SkipSpaces(results);
        
        float possibleValueAsFloat{ -1 };
        for (int i = results.lastPos; i < results.line.length() && results.line[i] != L' '; i++)
            if (results.line[i] == L'.')
                possibleValueAsFloat = nss::ParseAsFloat(results);
        
        int possibleValue;
        if (possibleValueAsFloat < 0 || possibleValueAsFloat > 1)
        {
            possibleValue = nss::ParseAsInt(results);
            if (possibleValue >= 0 && possibleValue <= 255)
                return possibleValue;
        }
        else
        {
            possibleValue = 255 * possibleValueAsFloat;
            return possibleValue;
        }
        
        return -1;
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
    /*wchar_t** ParseArguments(CommandSettings& results)
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
    }*/
    void ParseArguments(CommandSettings& results, std::vector<std::wstring>& vec)
    {
        while (results.lastPos+1 < results.line.length())
        {
            SkipSpaces(results);
            std::wstring argument = nss::ParseArgument(results);
            if (argument != L"with" && argument != L"as")
                vec.push_back(argument);
        }
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
    
    
    
    void SetStringWithLineBreaks(sf::Text& text, const std::wstring& line, unsigned int width, int shift)
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
    std::wstring GetStringWithLineBreaks(sf::Text& text, const std::wstring& line, unsigned int width, int shift)
    {
        sf::Text tempText = text;
        tempText.setString(line);
        if (tempText.getLocalBounds().width >= width)
        {
            std::wstring finalLine = L"";
            std::wstring currentLine = L"";
            bool shiftShifted{ false };
            for (int i = 0; i < line.length(); i++)
            {
                currentLine += line[i];
                tempText.setString(currentLine);
                if (tempText.getLocalBounds().width >= width)
                {
                    bool spaceFound{ false };
                    for (int j = currentLine.length() - 1; j >= 0 && !spaceFound; --j)
                    {
                        if ((spaceFound = (currentLine[j] == L' ')))
                        {
                            spaceFound = false;
                            for (int k = j - 1; k >= 0 && !spaceFound; --k)
                                if (currentLine[k] != L' ')
                                    spaceFound = true;
                            
                            if (spaceFound)
                            {
                                std::wstring toFinalLine;
                                for (int k = 0; k <= j; k++)
                                    toFinalLine += currentLine[k];
                                toFinalLine += L'\n';
                                finalLine += toFinalLine;
                                
                                if (shift != 0 && !shiftShifted)
                                    width -= shiftShifted, shiftShifted = true;
                                std::wstring newCurrentLine = L"";
                                if (shift != 0 && shift < width/4)
                                {
                                    tempText.setString(L"");
                                    while (tempText.getLocalBounds().width < shift)
                                        newCurrentLine += L' ', tempText.setString(newCurrentLine);
                                }
                                for (int k = j + 1; k < currentLine.length(); k++)
                                    newCurrentLine += currentLine[k];
                                
                                currentLine = newCurrentLine;
                            }
                        }
                    }
                    if (!spaceFound)
                    {
                        currentLine[currentLine.length() - 1] = L'\n';
                        finalLine += currentLine;
                        
                        if (shift != 0 && !shiftShifted)
                            width -= shiftShifted, shiftShifted = true;
                        if (shift != 0 && shift < width/10)
                        {
                            tempText.setString(L"");
                            while (tempText.getLocalBounds().width < shift)
                                currentLine += L' ', tempText.setString(currentLine);
                        }
                        currentLine = L"";
                        
                        currentLine += line[i];
                    }
                }
            }
            finalLine += currentLine;
            return finalLine;
        }
        return std::wstring(line);
    }
    std::wstring GetStringWithLineBreaksWOSpaceFinding(sf::Text& text, const std::wstring& line, const unsigned int width)
    {
        sf::Text tempText = text;
        tempText.setString(line);
        if (tempText.getLocalBounds().width >= width)
        {
            std::wstring finalLine = L"";
            std::wstring currentLine = L"";
            for (int i = 0; i < line.length(); i++)
            {
                currentLine += line[i];
                tempText.setString(currentLine);
                if (tempText.getLocalBounds().width >= width)
                {
                    currentLine[currentLine.length() - 1] = L'\n';
                    finalLine += currentLine;
                    currentLine = L"";
                    
                    currentLine += line[i];
                }
            }
            return finalLine;
        }
        return std::wstring(line);
    }
    
    
    
    
    //TODO: ^ operation and support for sin, cos, max etc functions.
    float MathParser(const std::wstring& line)
    {
        bool operatorWasFound{ false };
        ns::Stack<std::wstring> output;
        ns::Stack<std::wstring> operators;
        
        size_t wordLength = 0;
        size_t wordBeg = 0;
        for (int i = 0; i <= line.length(); ++i)
        {
            if (line[i] == L'/' || line[i] == L'+' || (line[i] == L'-') || line[i] == L'*' ||
                line[i] == L'(' || line[i] == L')' || line[i] == '\0')
            {
                if (wordLength != 0)
                {
                    std::wstring wstr = L"";
                    bool notANumber{ false };
                    for (int j = wordBeg; j < i; ++j)
                    {
                        if (line[j] != '\t' && line[j] != ' ')
                        {
                            if (!((line[j] >= 48 && line[j] <= 57) || line[j] == 46))
                                notANumber = true;
                            wstr += line[j];
                        }
                    }
                    
                    if (wstr.length() != 0)
                    {
                        if (notANumber)
                        {
                            /*if (wstr == L"sin" || wstr == L"cos")
                               operators.Push(wstr);
                            else*/
                            cout << "Warning :: MathParser :: not an operator." << endl;
                        }
                        else
                        {
                            bool makeNegative = false;
                            if (operators.size == 1 && output.size == 0)
                                if (operators[0][0] == '-')
                                    makeNegative = true, operators.Pop();
                            
                            if (makeNegative)
                                wstr.insert(wstr.begin(), L'-'), makeNegative = false;
                            output.Push(wstr);
                        }
                    }
                    wordLength = 0;
                }
                wordBeg = i + 1;
                
                if (line[i] != '\0')
                {
                    std::wstring wstr = L"";
                    wstr += line[i];
                    
                    if (line[i] == L')')
                    {
                        while (!operators.IsEmpty() && operators[0] != L"(")
                            output.Push(operators.Pop());
                        
                        if (!operators.IsEmpty())
                            operators.Pop();
                    }
                    else if (line[i] != L'(')
                    {
                        operatorWasFound = true;
                        
                        bool lowerFound{ false };
                        while (!operators.IsEmpty() && !lowerFound)
                        {
                            std::wstring op = operators[0];
                            if ((line[i] == '+' && op == L"-") || (line[i] == '-' && op == L"-") ||
                                (line[i] == '+' && op == L"+") || (line[i] == '-' && op == L"+"))
                                output.Push(operators.Pop());
                            else if ((line[i] == '*' && op == L"/") || (line[i] == '/' && op == L"/") ||
                                     (line[i] == '*' && op == L"*") || (line[i] == '/' && op == L"*"))
                                output.Push(operators.Pop());
                            else if ((line[i] == '+' && op == L"*") || (line[i] == '+' && op == L"/") ||
                                (line[i] == '-' && op == L"*") || (line[i] == '-' && op == L"/"))
                                output.Push(operators.Pop());
                            else
                                lowerFound = true;
                        }
                    }
                    
                    if (line[i] != L')')
                        operators.Push(wstr);
                }
            }
            else
                wordLength++;
        }
        
        if (!operatorWasFound)
        {
            if (output.IsEmpty())
                return 0;
            else
                return ns::base::ConvertToFloat(output[0]);
        }
        
        while (!operators.IsEmpty())
            output.Push(operators.Pop());
        
        /*cout << "Shunting-yard Algorithm's output: ";
        for (int j = output.size - 1; j >= 0; --j)
            cout << ns::base::ConvertToUTF8(output[j]) << " ";
        cout << endl;*/
        
        ns::Stack<std::wstring> values;
        int pos = output.size;
        while (pos >= 0)
        {
            std::wstring token = output[pos];
            bool notANumber{ false };
            bool possibleNegative{ false };
            for (int i = 0; i < token.length(); ++i)
            {
                if (token[i] != ' ' && token[i] != '\t')
                {
                    if (token[i] == '-')
                        possibleNegative = true;
                    else if (!((token[i] >= 48 && token[i] <= 57) || token[i] == 46))
                        notANumber = true;
                    else
                        possibleNegative = false;
                }
            }
            if (possibleNegative)
                notANumber = true;
            
            if (notANumber)
            {
                if (token == L"+" || token == L"-" || token == L"*" || token == L"/")
                {
                    if (!values.IsEmpty())
                    {
                        std::wstring roperand = values.Pop();
                        if (!values.IsEmpty())
                        {
                            std::wstring loperand = values.Pop();
                            float rop = ns::base::ConvertToFloat(roperand);
                            float lop = ns::base::ConvertToFloat(loperand);
                            
                            float result{ 0 };
                            if (token == L"+")
                                result = lop + rop;
                            else if (token == L"-")
                                result = lop - rop;
                            else if (token == L"*")
                                result = lop * rop;
                            else if (token == L"/")
                                result = lop / rop;
                            
                            values.Push(std::to_wstring(result));
                        }
                        else
                        {
                            cout << "Error :: MathParser :: Operation can't be done: '" << ns::base::utf8(token) << "'." << endl;
                            values.Push(roperand);
                        }
                    }
                    else
                        cout << "Error :: MathParser :: Operation can't be done: '" << ns::base::utf8(token) << "'." << endl;
                }
                else
                    cout << "Error :: MathParser :: Operation is not supported: '" << ns::base::utf8(token) << "'." << endl;
            }
            else
                values.Push(token);
            
            --pos;
        }
        
        if (!values.IsEmpty())
            return ns::base::ConvertToFloat(values.Pop());
        
        return 0;
    }
    
    /*
    float MathParser(const std::wstring& finalLine)
    {
        //math parsing
        wchar_t action = L' ';
        float left = 0;
        int numStartPos = 0;
        
        //TODO: Mind the operation order: (* and /) BIGGER THAN (+ and -)
        for (int i = 0; i <= finalLine.length(); i++)
        {
            if (finalLine[i] == L'/' || finalLine[i] == L'+' || finalLine[i] == L'-' || finalLine[i] == L'*' || finalLine[i] == '\0')
            {
                if (action == L' ')
                {
                    std::wstring leftS = L"";
                    for (int j = 0; j < i; j++)
                        leftS += finalLine[j];
                    left = ns::base::ConvertToFloat(leftS);
                }
                else
                {
                    std::wstring leftS = L"";
                    for (int j = numStartPos; j < i; j++)
                        leftS += finalLine[j];
                    float right = ns::base::ConvertToFloat(leftS);
                    
                    switch (action)
                    {
                        case L'+':
                            left = left + right;
                            break;
                        case L'-':
                            left = left - right;
                            break;
                        case L'*':
                            left = left * right;
                            break;
                        case L'/':
                            left = left / right;
                            break;
                        default:
                            break;
                    }
                }
                
                numStartPos = i + 1;
                action = finalLine[i];
            }
        }
        
        return left;
    }
    */
}
