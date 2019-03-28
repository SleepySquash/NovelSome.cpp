//
//  Skin.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "Skin.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Skin* Skin::self = nullptr;
        
        void Skin::RestoreToDefaults()
        {
            //TODO: Defaults
        }
        void Skin::LoadFromFile(const std::wstring& fileName)
        {
            std::wstring fullPath = fileName;
            if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
            
            std::wifstream wif;
#ifdef _WIN32
            wif.open(fullPath);
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(fullPath);
            wif.open(u8str);
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            
            bool fileOpened{ false };
            if (!((fileOpened = wif.is_open())))
                cout << "Error :: Skin :: File couldn't be opened, path: " << base::utf8(fileName) << endl;
            else
            {
                bool eof{ false };
                std::wstring line;
                nss::CommandSettings command;
                
                std::wstring guiScope = L"";
                std::wstring settingScope = L"";
                
                while (!eof)
                {
                    if (!wif.eof())
                    {
                        std::getline(wif, line);
                        command.Command(line);
                        
                        if (nss::Command(command, L"//")) { /* that's a comment */ }
                        else if (nss::Command(command, L"gui "))
                        {
                            int openedBrackets = 0;
                            for (int i = command.lastPos; i < line.length(); i++)
                                if (line[i] == L'{') openedBrackets++;
                            bool mightBeOnNextLine{ (openedBrackets == 0) };
                            while ((mightBeOnNextLine || openedBrackets > 0) && !wif.eof())
                            {
                                std::getline(wif, line);
                                for (int i = 0; i < line.length(); i++)
                                {
                                    if (line[i] == L'{') openedBrackets++;
                                    else if (line[i] == L'}') openedBrackets--;
                                }
                                if (openedBrackets > 0) mightBeOnNextLine = false;
                            }
                        }
                        else if (nss::Command(command, L"appeartime:") || nss::Command(command, L"appeartime "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0)
                            {
                                if (settingScope == L"music") music.appearTime = possibleValue;
                                else if (settingScope == L"ambient") ambient.appearTime = possibleValue;
                                else if (settingScope == L"sound") sound.appearTime = possibleValue;
                                else if (settingScope == L"background") background.appearTime = possibleValue;
                                else if (settingScope == L"character") character.appearTime = possibleValue;
                                else if (settingScope == L"dialogue") dialogue.appearTime = possibleValue;
                                else if (settingScope == L"choose") choose.appearTime = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"disappeartime:") || nss::Command(command, L"disappeartime "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0)
                            {
                                if (settingScope == L"music") music.disappearTime = possibleValue;
                                else if (settingScope == L"ambient") ambient.disappearTime = possibleValue;
                                else if (settingScope == L"sound") sound.disappearTime = possibleValue;
                                else if (settingScope == L"background") background.disappearTime = possibleValue;
                                else if (settingScope == L"character") character.disappearTime = possibleValue;
                                else if (settingScope == L"dialogue") dialogue.disappearTime = possibleValue;
                                else if (settingScope == L"choose") choose.disappearTime = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"maxalpha:") || nss::Command(command, L"maxalpha ") ||
                                 nss::Command(command, L"alpha:") || nss::Command(command, L"alpha "))
                        {
                            int possibleValue = nss::ParseAlpha(command);
                            if (possibleValue != -1)
                            {
                                if (settingScope == L"background")
                                    background.maxAlpha = possibleValue;
                                else if (settingScope == L"character")
                                    character.maxAlpha = possibleValue;
                                else if (settingScope == L"dialogue")
                                    dialogue.maxAlpha = possibleValue;
                                else if (settingScope == L"choose")
                                    choose.maxAlpha = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"maxvolume:") || nss::Command(command, L"maxvolume ") ||
                                 nss::Command(command, L"volume:") || nss::Command(command, L"volume "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0)
                            {
                                if (settingScope == L"music") music.maxVolume = possibleValue;
                                else if (settingScope == L"ambient") ambient.maxVolume = possibleValue;
                                else if (settingScope == L"sound") sound.maxVolume = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"charactersize:") || nss::Command(command, L"charactersize "))
                        {
                            nss::SkipSpaces(command);
                            int possibleValue = nss::ParseAsInt(command);
                            if (possibleValue > 0) dialogue.characterSize = possibleValue;
                        }
                        else if (nss::Command(command, L"size:") || nss::Command(command, L"size "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                int possibleValue = nss::ParseAsInt(command);
                                if (possibleValue > 0) dialogue.characterSize = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"characterinsecond:") || nss::Command(command, L"characterinsecond ") ||
                                 nss::Command(command, L"characterspeed:") || nss::Command(command, L"characterspeed "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0) dialogue.characterInSecond = possibleValue;
                        }
                        else if (nss::Command(command, L"insecond:") || nss::Command(command, L"insecond ")
                                 || nss::Command(command, L"speed:") || nss::Command(command, L"speed "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                float possibleValue = nss::ParseAsFloat(command);
                                if (possibleValue >= 0) dialogue.characterInSecond = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"mode:") || nss::Command(command, L"mode ") ||
                                 nss::Command(command, L"appearmode:") || nss::Command(command, L"appearmode ") ||
                                 nss::Command(command, L"textmode:") || nss::Command(command, L"textmode "))
                        {
                            nss::SkipSpaces(command);
                            std::wstring possibleValue = nss::ParseAsMaybeQuoteString(command);
                            if (possibleValue.length() != 0)
                            {
                                if (possibleValue == L"appearing" || possibleValue == L"fading" || possibleValue == L"appear" || possibleValue == L"fade" || possibleValue == L"fadein" || possibleValue == L"fadings" || possibleValue == L"alpha")
                                    dialogue.textAppearMode = 0;
                                else if (possibleValue == L"printing" || possibleValue == L"print" || possibleValue == L"character" || possibleValue == L"characters" || possibleValue == L"bycharacter" || possibleValue == L"bycharacters" || possibleValue == L"insecond" || possibleValue == L"inseconds" || possibleValue == L"second" || possibleValue == L"seconds" || possibleValue == L"bysecond" || possibleValue == L"byseconds" || possibleValue == L"speed" || possibleValue == L"characterspeed" || possibleValue == L"characterinsecond")
                                    dialogue.textAppearMode = 1;
                            }
                        }
                        else if (nss::Command(command, L"forcePressInsideDialogue:") || nss::Command(command, L"forcePressInsideDialogue "))
                        {
                            nss::SkipSpaces(command);
                            dialogue.forcePressInsideDialogue = nss::ParseAsBool(command);
                        }
                        else if (nss::Command(command, L"fontName:") || nss::Command(command, L"fontName ") ||
                                 nss::Command(command, L"font:") || nss::Command(command, L"font "))
                        {
                            nss::SkipSpaces(command);
                            std::wstring possibleValue = nss::ParseAsMaybeQuoteString(command);
                            if (possibleValue.length() != 0)
                            {
                                if (settingScope == L"dialogue") dialogue.fontName = possibleValue;
                                else if (settingScope == L"choose") choose.fontName = possibleValue;
                                else defaultFontName = possibleValue, dialogue.fontName = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"onleft:") || nss::Command(command, L"onleft ") ||
                                 nss::Command(command, L"ladd:") || nss::Command(command, L"ladd "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                if (command.line[command.lastPos] != 0)
                                    dialogue.leftSpeechAddition = command.line[command.lastPos];
                            }
                        }
                        else if (nss::Command(command, L"onright:") || nss::Command(command, L"onright ") ||
                                 nss::Command(command, L"radd:") || nss::Command(command, L"radd "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                if (command.line[command.lastPos] != 0)
                                    dialogue.rightSpeechAddition = command.line[command.lastPos];
                            }
                        }
                        else if (nss::Command(command, L"leftspeechaddition:") || nss::Command(command, L"leftspeechaddition ") ||
                                 nss::Command(command, L"leftaddition:") || nss::Command(command, L"leftaddition ") ||
                                 nss::Command(command, L"laddition:") || nss::Command(command, L"laddition "))
                        {
                            nss::SkipSpaces(command);
                            if (command.line[command.lastPos] != 0)
                                dialogue.leftSpeechAddition = command.line[command.lastPos];
                        }
                        else if (nss::Command(command, L"rightspeechaddition:") || nss::Command(command, L"rightspeechaddition ") ||
                                 nss::Command(command, L"rightaddition:") || nss::Command(command, L"rightaddition ") ||
                                 nss::Command(command, L"raddition:") || nss::Command(command, L"raddition "))
                        {
                            nss::SkipSpaces(command);
                            if (command.line[command.lastPos] != 0)
                                dialogue.rightSpeechAddition = command.line[command.lastPos];
                        }
                        else if (nss::Command(command, L"shift:") || nss::Command(command, L"shift "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                dialogue.afterRedLineShift = nss::ParseAsInt(command);
                            }
                        }
                        else if (nss::Command(command, L"afterredlineshift:") || nss::Command(command, L"afterredlineshift ") ||
                                 nss::Command(command, L"redlineshift:") || nss::Command(command, L"redlineshift ") ||
                                 nss::Command(command, L"afterredline:") || nss::Command(command, L"afterredline "))
                        {
                            nss::SkipSpaces(command);
                            dialogue.afterRedLineShift = nss::ParseAsInt(command);
                        }
                        else if (nss::Command(command, L"thickness:") || nss::Command(command, L"thickness ") ||
                                 nss::Command(command, L"outlinethickness:") || nss::Command(command, L"outlinethickness "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                float possibleValue = nss::ParseAsFloat(command);
                                if (possibleValue >= 0) dialogue.outlineThickness = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"color:") || nss::Command(command, L"color ") ||
                                 nss::Command(command, L"fill:") || nss::Command(command, L"fill ") ||
                                 nss::Command(command, L"fillcolor:") || nss::Command(command, L"fillcolor "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                sf::Color possibleValue = nss::ParseColor(command);
                                if (possibleValue.a != 255) dialogue.fillColor = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"ocolor:") || nss::Command(command, L"ocolor ") ||
                                 nss::Command(command, L"outline:") || nss::Command(command, L"outline ") ||
                                 nss::Command(command, L"outlinecolor:") || nss::Command(command, L"outlinecolor "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                sf::Color possibleValue = nss::ParseColor(command);
                                if (possibleValue.a != 255) dialogue.outlineColor = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"music")) settingScope = L"music";
                        else if (nss::Command(command, L"sound")) settingScope = L"sound";
                        else if (nss::Command(command, L"dialogue")) settingScope = L"dialogue";
                        else if (nss::Command(command, L"choose")) settingScope = L"choose";
                        else if (nss::Command(command, L"background")) settingScope = L"background";
                        else if (nss::Command(command, L"ambient")) settingScope = L"ambient";
                        else if (nss::Command(command, L"character")) settingScope = L"character";
                    }
                    else eof = true;
                }
            }
            wif.close();
        }
    }
}
