//
//  NovelComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Novel::Novel(sf::String path) : nsdataPath(path)
        {
            //ns::ic::FreeImages();
            
            folderPath = ns::base::GetFolderPath(path);
            
            localVariables.insert({L"@dialogue", new NovelVariable(std::wstring(L""))});
            localVariables.insert({L"@name", new NovelVariable(std::wstring(L""))});
            localVariables.insert({L"version", new NovelVariable(std::wstring(L"Update 0 build 16"))});
            
            library.SetNovel(this);
            library.ScanForCharacters();
            
            gamePause.novel = this;
            skin.dialogue.gui.SetNovel(this);
            skin.LoadFromFile(folderPath + L"skin.nskin");
            
            sf::String filePath = (resourcePath() + path);
#ifdef _WIN32
            wif.open(filePath.toWideString());
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(filePath.toWideString());
            wif.open(u8str);
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            
            if (!(fileOpened = wif.is_open()))
                cout << "Error :: NovelComponent :: File couldn't be opened, path: " << path.toAnsiString() << endl;
        }
        Novel::~Novel()
        {
            wif.close();
            layers.clear();
            skin.dialogue.gui.Clear();
            skin.gamePauseGUI.Clear();
            ns::ic::FreeImages();
            
            for (auto& key : localVariables)
                if (key.second != nullptr)
                    delete key.second;
            
            FreeGroup<Background>(backgroundGroup);
            FreeGroup<Character>(characterGroup);
            FreeGroup<Dialogue>(dialogueGroup);
            FreeGroup<SoundPlayer>(soundGroup);
            FreeGroup<MusicPlayer>(musicGroup);
            FreeGroup<GUISystem>(GUIGroup);
            
            FreeGroup<NovelObject>(onHold);
        }
        // TODO: Make it possible to ignore some lines of novel
        void Novel::ResourcesPreloading(list<std::wstring>& lines, std::wstring& line)
        {
            while (lines.size() <= preloadLinesAmount && !wif.eof())
            {
                std::getline(wif, line); //cout << "ANALYZING " << base::utf8(line);
                nss::CommandSettings settings;
                settings.Command(line);
                
                bool shouldPush{ true };
                if (nss::Command(settings, L"choose")) { lines.push_front(line); break; }
                else if (nss::Command(settings, L"//")) shouldPush = false;
                else if (!nss::ContainsUsefulInformation(line)) shouldPush = false;
                else if (nss::Command(settings, L"background hide")) { }
                else if (nss::Command(settings, L"hide ")) { }
                else if (nss::Command(settings, L"background add ") || nss::Command(settings, L"background "))
                {
                    std::wstring filePath = nss::ParseAsQuoteString(settings);
                    ic::PreloadTexture(folderPath + filePath, 1);
                    //cout << ">>>>> background preload" << endl;
                }
                else if (nss::Command(settings, L"show "))
                {
                    //cout << ">>>>> character preload" << endl;
                    std::wstring possibleName = nss::ParseUntil(settings, ' ');
                    if (possibleName.length() != 0)
                    {
                        if (library.characterLibrary.find(possibleName) != library.characterLibrary.end())
                        {
                            std::wstring state{ L"" };
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(settings, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument;
                                argument.Command(*it);
                                
                                if (nss::Command(argument, L"fade:")) { }
                                else if (nss::Command(argument, L"fadein:") || nss::Command(argument, L"appear:")) { }
                                else if (nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:")) { }
                                else if (nss::Command(argument, L"alpha:") || nss::Command(argument, L"maxalpha:")) { }
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:")) { }
                                else
                                {
                                    std::wstring possibleStateOrPos;
                                    if (argument.line[0] == L'"')
                                        possibleStateOrPos = nss::ParseAsQuoteString(argument);
                                    else possibleStateOrPos = argument.line;
                                    
                                    if (state.length() == 0) state = possibleStateOrPos;
                                }
                            }
                            
                            CharacterData* characterData = library.characterLibrary[possibleName];
                            sf::String fullPath = sf::String(resourcePath()) + folderPath + characterData->filePath;
                            std::wstring lookForSpritePath = base::GetFolderPath(folderPath + characterData->filePath);
                            std::wstring spritePath{ L"" };
                            
                            std::wifstream wifc;
#ifdef _WIN32
                            wifc.open(fullPath.toWideString());
#else
                            std::wstring _wpath = fullPath;
                            std::string _path(_wpath.begin(), _wpath.end());
                            wifc.open(_path);
#endif
                            wifc.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                            
                            if (wifc.is_open())
                            {
                                bool eofc{ false };
                                std::wstring linec;
                                nss::CommandSettings commandc;
                                bool stateReading{ false };
                                
                                while (!eofc)
                                {
                                    if (!wifc.eof())
                                    {
                                        std::getline(wifc, linec); commandc.Command(linec);
                                        if (!stateReading)
                                        {
                                            if (nss::Command(commandc, L"state ") || nss::Command(commandc, L"sprite "))
                                            {
                                                nss::SkipSpaces(commandc); std::wstring stateName;
                                                if (commandc.line[commandc.lastPos] == L'"')
                                                    stateName = nss::ParseAsQuoteString(commandc);
                                                else stateName = nss::ParseUntil(commandc, ' ');
                                                
                                                stateReading = (stateName == state) ? true : false;
                                            }
                                        } else {
                                            if (nss::Command(commandc, L"state ") || nss::Command(commandc, L"end") || nss::Command(commandc, L"}"))
                                                stateReading = false;
                                            else if (nss::Command(commandc, L"image ") || nss::Command(commandc, L"sprite "))
                                                spritePath = nss::ParseAsQuoteString(commandc);
                                            else if (nss::Command(commandc, L"\""))
                                            {
                                                commandc.lastPos--;
                                                spritePath = nss::ParseAsQuoteString(commandc);
                                            }
                                            
                                            eofc = stateReading ? false : true;
                                        }
                                    } else eofc = true;
                                }
                            }
                            wifc.close();
                            if (spritePath.length() != 0) ic::PreloadTexture(lookForSpritePath + spritePath, 2);
                        }
                    }
                }
                else if (nss::Command(settings, L"sound "))
                {
                    std::wstring filePath = nss::ParseAsQuoteString(settings);
                    sc::PreloadSound(folderPath + filePath);
                    //cout << ">>>>> sound preload" << endl;
                }
                
                if (shouldPush)
                {
                    lines.push_front(line);
                    //cout << "ADDING " << base::utf8(line);
                }
            }
        }
        void Novel::Draw(sf::RenderWindow* window)
        {
            if (fileOpened)
                layers.Draw(window);
            gamePause.Draw(window);
        }
        void Novel::Resize(unsigned int width, unsigned int height)
        {
            if (fileOpened)
                layers.Resize(width, height);
            gamePause.Resize(width, height);
        }
        void Novel::PollEvent(sf::Event& event)
        {
            if (fileOpened && !(ns::GlobalSettings::isPause))
                layers.PollEvent(event);
            gamePause.PollEvent(event);
        }
        void Novel::OnHold(NovelObject* component)
        {
            onHold.insert(onHold.begin(), component);
            onHoldSize = onHold.size();
        }
        void Novel::UnHold(NovelObject* component)
        {
            std::list<NovelObject*>::iterator it = std::find(onHold.begin(), onHold.end(), component);
            if (it != onHold.end())
            {
                onHold.erase(it);
                onHoldSize = onHold.size();
            }
        }
        sf::String Novel::GetFolderPath()
        {
            return folderPath;
        }
        
        
        
        
        
        
        NovelVariable* Novel::FindVariable(const std::wstring& variableName)
        {
            return (localVariables.find(variableName) != localVariables.end()) ? localVariables.at(variableName) : nullptr;
        }
        void Novel::VariableChange(const std::wstring& name)
        {
            skin.dialogue.gui.VariableChange(name);
            skin.choose.guiChoose.VariableChange(name);
            skin.gamePauseGUI.VariableChange(name);
            if (GUIGroup.size() != 0)
                for (auto g : GUIGroup)
                    g->VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, std::wstring value)
        {
            NovelVariable *nvar;
            if (localVariables.find(name) == localVariables.end())
            {
                nvar = new NovelVariable();
                localVariables.insert({name, nvar});
            }
            else
                nvar = localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
            {
                if (nvar->type == nvar->String)
                    callChange = (nvar->value.asString != value);
                else
                    callChange = true;
            }
            
            nvar->Set(value);
            if (callChange)
                VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, bool value)
        {
            NovelVariable *nvar;
            if (localVariables.find(name) == localVariables.end())
            {
                nvar = new NovelVariable();
                localVariables.insert({name, nvar});
            }
            else
                nvar = localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
                if (!(callChange = (nvar->type != nvar->Boolean)))
                    callChange = (nvar->value.asBoolean != value);
            
            nvar->Set(value);
            if (callChange)
                VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, int value)
        {
            NovelVariable *nvar;
            if (localVariables.find(name) == localVariables.end())
            {
                nvar = new NovelVariable();
                localVariables.insert({name, nvar});
            }
            else
                nvar = localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
                if (!(callChange = (nvar->type != nvar->Integer)))
                    callChange = (nvar->value.asInt != value);
            
            nvar->Set(value);
            if (callChange)
                VariableChange(name);
        }
        
        
        
        
        
        
        void Novel::RemoveFromGroup(const list<Background*>::iterator& groupPointer) { backgroundGroup.erase(groupPointer); }
        void Novel::RemoveFromGroup(const list<Dialogue*>::iterator& groupPointer) { dialogueGroup.erase(groupPointer); }
        void Novel::RemoveFromGroup(const list<Choose*>::iterator& groupPointer) { chooseGroup.erase(groupPointer); }
        void Novel::RemoveFromGroup(const list<Character*>::iterator& groupPointer) { characterGroup.erase(groupPointer); }
        void Novel::RemoveFromGroup(const list<SoundPlayer*>::iterator& groupPointer) { soundGroup.erase(groupPointer); }
        void Novel::RemoveFromGroup(const list<MusicPlayer*>::iterator& groupPointer) { musicGroup.erase(groupPointer); }
        void Novel::RemoveFromGroup(const list<GUISystem*>::iterator& groupPointer) { GUIGroup.erase(groupPointer); }
    }
}

/*void NovelLibrary::ScanForCharacters()
 {
 if (novel != nullptr)
 {
 DIR *dir;
 struct dirent *ent;
 
 sf::String path{ executablePath() + novel->GetFolderPath() };
 if ((dir = opendir(path.toAnsiString().c_str())) != NULL)
 {
 while ((ent = readdir (dir)) != NULL)
 std::cout << "Entry found: " << ent->d_name << std::endl;
 closedir (dir);
 }
 else
 std::cout << "NovelLibrary :: ScanForCharacters :: Could not open directory: '" << path.toAnsiString() << '\'' << std::endl;
 }
 else
 std::cout << "NovelLibrary :: ScanForCharacters :: Novel not set: '" << '\'' << std::endl;
 }*/
