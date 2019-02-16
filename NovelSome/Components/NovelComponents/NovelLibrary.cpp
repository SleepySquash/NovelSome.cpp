//
//  NovelLibrary.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Skin::RestoreToDefaults()
        {
            //TODO: Defaults
            dialogue.gui.Clear();
            GUIObjects::Rectangle* dialogueRect = dialogue.gui.AddComponent<GUIObjects::Rectangle>();
            dialogue.dialogueRect = dialogueRect;
            dialogueRect->shape.setFillColor(sf::Color::Black);
            dialogueRect->maxAlpha = 170;
            dialogueRect->constrains.leftS = L"30";
            dialogueRect->constrains.rightS = L"30";
            dialogueRect->constrains.bottomS = L"10";
            dialogueRect->constrains.heightS = L".height/5 - 10";
            
            GUISystem* childSystem = dialogueRect->GetChildSystem();
            GUIObjects::Rectangle* nameRect = childSystem->AddComponent<GUIObjects::Rectangle>();
            dialogue.nameRect = nameRect;
            nameRect->shape.setFillColor(sf::Color::Black);
            nameRect->maxAlpha = 170;
            nameRect->constrains.leftS = L"10";
            nameRect->constrains.rightS = L"0";
            nameRect->constrains.bottomS = L".height + 5";
            nameRect->constrains.widthS = L"@name.width + 20";
            nameRect->constrains.heightS = L"@name.height + 10";
            nameRect->SetFadings(GUIObject::offline);
            
            
            choose.guiChoose.Clear();
            GUIObjects::Rectangle* chooseRect = choose.guiChoose.AddComponent<GUIObjects::Rectangle>();
            choose.chooseRect = chooseRect;
            chooseRect->shape.setFillColor(sf::Color::Black);
            chooseRect->maxAlpha = 170;
            chooseRect->constrains.posXS = L".width/2 - 240";
            chooseRect->constrains.posYS = L".height/2 - 160";
            chooseRect->constrains.widthS = L"480";
            chooseRect->constrains.heightS = L"320";
            
            
            gamePauseGUI.Clear();
            GUIObjects::Rectangle* pauseRect = gamePauseGUI.AddComponent<GUIObjects::Rectangle>();
            pauseRect->shape.setFillColor(sf::Color::Black);
            pauseRect->maxAlpha = 170;
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
                                if (line[i] == L'{')
                                    openedBrackets++;
                            bool mightBeOnNextLine{ (openedBrackets == 0) };
                            while ((mightBeOnNextLine || openedBrackets > 0) && !wif.eof())
                            {
                                std::getline(wif, line);
                                for (int i = 0; i < line.length(); i++)
                                {
                                    if (line[i] == L'{')
                                        openedBrackets++;
                                    else if (line[i] == L'}')
                                        openedBrackets--;
                                }
                                if (openedBrackets > 0)
                                    mightBeOnNextLine = false;
                            }
                        }
                        else if (nss::Command(command, L"appeartime:") || nss::Command(command, L"appeartime "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0)
                            {
                                if (settingScope == L"music")
                                    music.appearTime = possibleValue;
                                else if (settingScope == L"ambient")
                                    ambient.appearTime = possibleValue;
                                else if (settingScope == L"sound")
                                    sound.appearTime = possibleValue;
                                else if (settingScope == L"background")
                                    background.appearTime = possibleValue;
                                else if (settingScope == L"character")
                                    character.appearTime = possibleValue;
                                else if (settingScope == L"dialogue")
                                    dialogue.appearTime = possibleValue;
                                else if (settingScope == L"choose")
                                    choose.appearTime = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"disappeartime:") || nss::Command(command, L"disappeartime "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0)
                            {
                                if (settingScope == L"music")
                                    music.disappearTime = possibleValue;
                                else if (settingScope == L"ambient")
                                    ambient.disappearTime = possibleValue;
                                else if (settingScope == L"sound")
                                    sound.disappearTime = possibleValue;
                                else if (settingScope == L"background")
                                    background.disappearTime = possibleValue;
                                else if (settingScope == L"character")
                                    character.disappearTime = possibleValue;
                                else if (settingScope == L"dialogue")
                                    dialogue.disappearTime = possibleValue;
                                else if (settingScope == L"choose")
                                    choose.disappearTime = possibleValue;
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
                                if (settingScope == L"music")
                                    music.maxVolume = possibleValue;
                                else if (settingScope == L"ambient")
                                    ambient.maxVolume = possibleValue;
                                else if (settingScope == L"sound")
                                    sound.maxVolume = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"charactersize:") || nss::Command(command, L"charactersize "))
                        {
                            nss::SkipSpaces(command);
                            int possibleValue = nss::ParseAsInt(command);
                            if (possibleValue > 0)
                                dialogue.characterSize = possibleValue;
                        }
                        else if (nss::Command(command, L"size:") || nss::Command(command, L"size "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                int possibleValue = nss::ParseAsInt(command);
                                if (possibleValue > 0)
                                    dialogue.characterSize = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"characterinsecond:") || nss::Command(command, L"characterinsecond ") ||
                                 nss::Command(command, L"characterspeed:") || nss::Command(command, L"characterspeed "))
                        {
                            nss::SkipSpaces(command);
                            float possibleValue = nss::ParseAsFloat(command);
                            if (possibleValue >= 0)
                                dialogue.characterInSecond = possibleValue;
                        }
                        else if (nss::Command(command, L"insecond:") || nss::Command(command, L"insecond ")
                                 || nss::Command(command, L"speed:") || nss::Command(command, L"speed "))
                        {
                            if (settingScope == L"dialogue")
                            {
                                nss::SkipSpaces(command);
                                float possibleValue = nss::ParseAsFloat(command);
                                if (possibleValue >= 0)
                                    dialogue.characterInSecond = possibleValue;
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
                            bool possibleValue = nss::ParseAsBool(command);
                            
                            dialogue.forcePressInsideDialogue = possibleValue;
                        }
                        else if (nss::Command(command, L"fontName:") || nss::Command(command, L"fontName ") ||
                                 nss::Command(command, L"font:") || nss::Command(command, L"font "))
                        {
                            nss::SkipSpaces(command);
                            std::wstring possibleValue = nss::ParseAsMaybeQuoteString(command);
                            if (possibleValue.length() != 0)
                            {
                                if (settingScope == L"dialogue")
                                    dialogue.fontName = possibleValue;
                                else if (settingScope == L"choose")
                                    choose.fontName = possibleValue;
                                else
                                    defaultFontName = possibleValue, dialogue.fontName = possibleValue;
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
                                if (possibleValue >= 0)
                                    dialogue.outlineThickness = possibleValue;
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
                                if (possibleValue.a != 255)
                                    dialogue.fillColor = possibleValue;
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
                                if (possibleValue.a != 255)
                                    dialogue.outlineColor = possibleValue;
                            }
                        }
                        else if (nss::Command(command, L"music"))
                            settingScope = L"music";
                        else if (nss::Command(command, L"sound"))
                            settingScope = L"sound";
                        else if (nss::Command(command, L"dialogue"))
                            settingScope = L"dialogue";
                        else if (nss::Command(command, L"choose"))
                            settingScope = L"choose";
                        else if (nss::Command(command, L"background"))
                            settingScope = L"background";
                        else if (nss::Command(command, L"ambient"))
                            settingScope = L"ambient";
                        else if (nss::Command(command, L"character"))
                            settingScope = L"character";
                    }
                    else
                        eof = true;
                }
            }
            wif.close();
            
            bool loadDefaultGUI{ true };
            bool loadDefaultChoice{ true };
            bool loadDefaultPause{ true };
            if (fileOpened)
            {
                dialogue.gui.Clear();
                loadDefaultGUI = !dialogue.gui.LoadFromFile(fileName, this, L"dialogue");
                
                choose.guiChoose.Clear();
                loadDefaultChoice = !choose.guiChoose.LoadFromFile(fileName, this, L"choose");
                
                gamePauseGUI.Clear();
                loadDefaultPause = !gamePauseGUI.LoadFromFile(fileName, this, L"gamepause");
            }
            if (loadDefaultGUI)
            {
                //Adding the dialogue's box
                GUIObjects::Rectangle* dialogueRect = dialogue.gui.AddComponent<GUIObjects::Rectangle>();
                dialogue.dialogueRect = dialogueRect;
                dialogueRect->shape.setFillColor(sf::Color::Black);
                dialogueRect->maxAlpha = 170;
                dialogueRect->constrains.leftS = L"30";
                dialogueRect->constrains.rightS = L"30";
                dialogueRect->constrains.bottomS = L"10";
                dialogueRect->constrains.heightS = L".height/5 - 10";
                
                //Requesting the child system to add the name's box
                GUISystem* childSystem = dialogueRect->GetChildSystem();
                GUIObjects::Rectangle* nameRect = childSystem->AddComponent<GUIObjects::Rectangle>();
                dialogue.nameRect = nameRect;
                nameRect->shape.setFillColor(sf::Color::Black);
                nameRect->maxAlpha = 170;
                nameRect->constrains.leftS = L"10";
                nameRect->constrains.rightS = L"0";
                nameRect->constrains.bottomS = L".height + 5";
                nameRect->constrains.widthS = L"@name.width + 20";
                nameRect->constrains.heightS = L"@name.height + 10";
                nameRect->SetFadings(GUIObject::offline);
            }
            if (loadDefaultChoice)
            {
                GUIObjects::Rectangle* chooseRect = choose.guiChoose.AddComponent<GUIObjects::Rectangle>();
                choose.chooseRect = chooseRect;
                chooseRect->shape.setFillColor(sf::Color::Black);
                chooseRect->maxAlpha = 170;
                chooseRect->constrains.posXS = L".width/2 - 240";
                chooseRect->constrains.posYS = L".height/2 - 160";
                chooseRect->constrains.widthS = L"480";
                chooseRect->constrains.heightS = L"320";
            }
            if (loadDefaultPause)
            {
                GUIObjects::Rectangle* pauseRect = gamePauseGUI.AddComponent<GUIObjects::Rectangle>();
                pauseRect->shape.setFillColor(sf::Color::Black);
                pauseRect->maxAlpha = 170;
            }
        }
        
        
        
        
        
        NovelLibrary::NovelLibrary() { }
        NovelLibrary::~NovelLibrary()
        {
            FreeData();
        }
        void NovelLibrary::FreeData()
        {
            for (const auto& key : characterLibrary)
            {
                if (key.second != nullptr)
                    delete key.second;
            }
        }
        void NovelLibrary::ScanForCharacters()
        {
            if (novel != nullptr)
            {
#ifdef _WIN32
                _WDIR *dir;
                struct _wdirent *ent;
#define _opendir _wopendir
#define _readdir _wreaddir
#define _closedir _wclosedir
#else
                DIR *dir;
                struct dirent *ent;
#define _opendir opendir
#define _readdir readdir
#define _closedir closedir
#endif
                
                std::wstring novelPath{ novel->GetFolderPath() };
                if (!base::FileExists(novelPath)) novelPath = base::utf16(resourcePath()) + novelPath;
                
                list<std::wstring> folders;
                folders.push_back(L"");
                
                for (list<std::wstring>::iterator currentFolder = folders.begin(); currentFolder != folders.end(); ++currentFolder)
                {
                    std::wstring fullpath{ novelPath + *currentFolder };
#ifdef _WIN32
                    if ((dir = _opendir(fullpath.c_str())) != NULL)
#else
                        if ((dir = _opendir(base::utf8(fullpath).c_str())) != NULL)
#endif
                        {
                            while ((ent = _readdir(dir)) != NULL)
                            {
                                std::wstring entryName{ base::utf16(ent->d_name) };
                                if (entryName != L"." && entryName != L"..")
                                {
                                    std::wstring extention = ns::base::GetExtentionFromString(entryName);
                                    if (extention == L"") folders.push_back(*currentFolder + entryName + L"/");
                                    else if (extention == L".nschar")
                                    {
                                        std::wifstream wif;
                                        std::wstring filePath = (fullpath + entryName);
#ifdef _WIN32
                                        wif.open(filePath.toWideString());
#else
                                        wif.open(base::utf8(filePath));
#endif
                                        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                                        
                                        if (!wif.is_open())
                                            cout << "Warning :: NovelComponent :: File couldn't be opened, path: " << base::utf8(filePath) << endl;
                                        else
                                        {
                                            bool eof{ false };
                                            std::wstring line;
                                            nss::CommandSettings command;
                                            CharacterData* charData = new CharacterData();
                                            
                                            while (!eof)
                                            {
                                                if (!wif.eof())
                                                {
                                                    std::getline(wif, line);
                                                    command.Command(line);
                                                    
                                                    if (nss::Command(command, L"name "))
                                                    {
                                                        std::wstring nameParsed;
                                                        if (command.line[command.lastPos] == L'"')
                                                            nameParsed = nss::ParseAsQuoteString(command);
                                                        else
                                                            nameParsed = nss::ParseUntil(command, ' ');
                                                        
                                                        charData->name = nameParsed;
                                                    }
                                                    else if (nss::Command(command, L"dname ") || nss::Command(command, L"display "))
                                                    {
                                                        std::wstring nameParsed;
                                                        if (command.line[command.lastPos] == L'"')
                                                            nameParsed = nss::ParseAsQuoteString(command);
                                                        else
                                                            nameParsed = nss::ParseUntil(command, ' ');
                                                        charData->displayName = nameParsed;
                                                    }
                                                    else if (nss::Command(command, L"state "))
                                                    {
                                                        nss::ParseUntil(command, '"');
                                                        std::wstring stateName = nss::ParseUntil(command, '"');
                                                        //TODO: State parsing
                                                    }
                                                    else if (nss::Command(command, L"fill ") || nss::Command(command, L"fillcolor ") || nss::Command(command, L"color ") || nss::Command(command, L"colour "))
                                                    {
                                                        sf::Color possibleColor = nss::ParseColor(command);
                                                        if (possibleColor.a != 255)
                                                            charData->fillColor = possibleColor;
                                                    }
                                                    else if (nss::Command(command, L"outline ") || nss::Command(command, L"outlinecolor ") || nss::Command(command, L"ocolor ") || nss::Command(command, L"ocolour "))
                                                    {
                                                        sf::Color possibleColor = nss::ParseColor(command);
                                                        if (possibleColor.a != 255)
                                                            charData->outlineColor = possibleColor;
                                                    }
                                                    else if (nss::Command(command, L"thickness "))
                                                    {
                                                        std::wstring possibleThickness = nss::ParseUntil(command, '\0');
                                                        if (possibleThickness.length() != 0)
                                                            charData->outlineThickness = ns::base::atof(possibleThickness);
                                                    }
                                                }
                                                else
                                                    eof = true;
                                            }
                                            
                                            
                                            if (charData->name == L"")
                                            {
#ifdef _WIN32
                                                charData->name = ns::base::GetStringWithNoExtention(ent->d_name);
#else
                                                std::string entDNameString(ent->d_name);
                                                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                                                charData->name = base::GetStringWithNoExtention(converter.from_bytes(entDNameString));
#endif
                                            }
                                            if (charData->displayName == L"")
                                                charData->displayName = charData->name;
                                            
                                            charData->filePath = std::wstring(*currentFolder) + entryName;
                                            if (novel->library.characterLibrary.find(charData->name) != novel->library.characterLibrary.end())
                                            {
                                                std::cout << "Warning :: NovelLibrary :: ScanForCharacters :: Character with following name is already placed: '" << charData->name.toAnsiString() << '\'' << std::endl;
                                                
                                                //TODO: replaceCharacterIfExists as static ns::GlobalSettings
                                                bool replaceCharacterIfExists{ true }; //if not, then to the name should be added "name2" etc
                                                if (replaceCharacterIfExists)
                                                {
                                                    delete novel->library.characterLibrary.at(charData->name.toWideString());
                                                    novel->library.characterLibrary.emplace(charData->name.toWideString(), charData);
                                                }
                                            }
                                            else
                                                novel->library.characterLibrary.emplace(charData->name.toWideString(), charData);
                                        }
                                        wif.close();
                                    }
                                }
                            }
                            if (ent != NULL) free(ent);
                            ent = NULL;
                            _closedir(dir);
                        }
                        else std::cout << "Warning :: NovelLibrary :: ScanForCharacters :: Could not open directory: '" << base::utf8(fullpath) << '\'' << std::endl;
                }
            }
            else
                std::cout << "Error :: NovelLibrary :: ScanForCharacters :: Novel or library not set: '" << '\'' << std::endl;
        }
        void NovelLibrary::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        
        
        
        
        
        
        
        
        NovelVariable::~NovelVariable()
        {
            if (type == String)
                delete[] value.asString;
        }
        NovelVariable::NovelVariable() { }
        NovelVariable::NovelVariable(const int& value)
        {
            Set(value);
        }
        NovelVariable::NovelVariable(const std::wstring& value)
        {
            Set(value);
        }
        NovelVariable::NovelVariable(const bool& value)
        {
            Set(value);
        }
        void NovelVariable::Set(const int& value)
        {
            if (type == String)
                delete[] this->value.asString;
            
            type = Integer;
            this->value.asInt = value;
        }
        void NovelVariable::Set(const std::wstring& value)
        {
            if (type == String)
                delete[] this->value.asString;
            
            type = String;
            this->value.asString = (wchar_t*)malloc(sizeof(wchar_t) * (value.length() + 1));
            for (int i = 0; i <= value.length(); i++)
                this->value.asString[i] = value[i];
        }
        void NovelVariable::Set(const bool& value)
        {
            if (type == String)
                delete[] this->value.asString;
            
            type = Boolean;
            this->value.asBoolean = value;
        }
        std::ostream& operator<<(std::ostream& os, const NovelVariable& Var)
        {
            switch (Var.type)
            {
                case NovelVariable::NotSet:
                    os << "NotSet";
                    break;
                case NovelVariable::Integer:
                    os << Var.value.asInt;
                    break;
                case NovelVariable::Boolean:
                    os << (Var.value.asBoolean ? "true" : "false");
                    break;
                case NovelVariable::String:
                    os << base::utf8(std::wstring(Var.value.asString));
                    break;
            }
            return os;
        }
    }
}
