//
//  Character.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Character::LoadState(sf::String state)
        {
            spriteLoaded = false;
            if (novel != nullptr && characterData != nullptr)
            {
                sf::String fullPath = sf::String(resourcePath()) + novel->GetFolderPath() + characterData->filePath;
                std::wstring lookForSpritePath = ns::base::GetFolderPath(fullPath.toWideString());
                std::wstring spritePath{ L"" };
                
                float scaleFactorX{ 1.f }, scaleFactorY{ 1.f };
                
                std::wifstream wif;
#ifdef _WIN32
                wif.open(fullPath.toWideString());
#else
                std::wstring _wpath = fullPath;
                std::string _path(_wpath.begin(), _wpath.end());
                wif.open(_path);
#endif
                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                
                if (!wif.is_open())
                    cout << "Warning :: Character :: File couldn't be opened, path: " << fullPath.toAnsiString() << endl;
                else
                {
                    bool eof{ false };
                    std::wstring line;
                    nss::CommandSettings command;
                    
                    bool stateReading{ false };
                    
                    while (!eof)
                    {
                        if (!wif.eof())
                        {
                            std::getline(wif, line);
                            command.Command(line);
                            
                            if (!stateReading)
                            {
                                if (nss::Command(command, L"state ") || nss::Command(command, L"sprite "))
                                {
                                    nss::SkipSpaces(command);
                                    
                                    std::wstring stateName;
                                    if (command.line[command.lastPos] == L'"')
                                        stateName = nss::ParseAsQuoteString(command);
                                    else
                                        stateName = nss::ParseUntil(command, ' ');
                                    
                                    stateReading = (stateName == state.toWideString()) ? true : false;
                                }
                            }
                            else
                            {
                                if (nss::Command(command, L"state ") || nss::Command(command, L"end") || nss::Command(command, L"}"))
                                    stateReading = false;
                                else if (nss::Command(command, L"image ") || nss::Command(command, L"sprite "))
                                    spritePath = nss::ParseAsQuoteString(command);
                                else if (nss::Command(command, L"\""))
                                {
                                    command.lastPos--;
                                    spritePath = nss::ParseAsQuoteString(command);
                                }
                                else if (nss::Command(command, L"scale "))
                                {
                                    scaleFactorX = nss::ParseAsFloat(command);
                                    scaleFactorY = scaleFactorX;
                                }
                                else if (nss::Command(command, L"scalexy "))
                                {
                                    scaleFactorX = nss::ParseAsFloat(command);
                                    SkipSpaces(command);
                                    scaleFactorY = nss::ParseAsFloat(command);
                                }
                                else if (nss::Command(command, L"scalex "))
                                    scaleFactorX = nss::ParseAsFloat(command);
                                else if (nss::Command(command, L"scaley "))
                                    scaleFactorY = nss::ParseAsFloat(command);
                            }
                        }
                        else
                            eof = true;
                    }
                }
                wif.close();
                
                if (spritePath.length() != 0)
                {
                    sf::String filePath = lookForSpritePath + spritePath;
                    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                    std::string u8str = converter.to_bytes(filePath.toWideString());
                    
                    if (image.loadFromFile(u8str))
                    {
                        if (texture.loadFromImage(image))
                        {
                            spriteLoaded = true;
                            texture.setSmooth(true);
                            sprite.setTexture(texture);
                            sprite.setScale(scaleFactorX, scaleFactorY);
                            
                            Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
                        }
                    }
                }
                
                if (!spriteLoaded)
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
            }
            else
                cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Character::Resize(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height > height/sprite.getScale().y ? height/sprite.getScale().y : sprite.getLocalBounds().height);
                switch (position)
                {
                    case left:
                        sprite.setPosition((float)width/5, height);
                        break;
                        
                    case cleft:
                        sprite.setPosition((float)width/3, height);
                        break;
                        
                    case center:
                        sprite.setPosition((float)width/2, height);
                        break;
                        
                    case cright:
                        sprite.setPosition((width - (float)width/3), height);
                        break;
                        
                    case right:
                        sprite.setPosition((width - (float)width/5), height);
                        break;
                        
                    default:
                        sprite.setPosition(customX, customY);
                        break;
                }
            }
        }
        void Character::Update(const sf::Time& elapsedTime)
        {
            switch (mode)
            {
                case appearing:
                    if (currentTime < appearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= appearTime)
                    {
                        alpha = maxAlpha;
                        currentTime = 0.f;
                        mode = afterAppearSwitchTo;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atAppearance)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case disappearing:
                    if (currentTime < disappearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0;
                        currentTime = 0.f;
                        mode = deprecated;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDeprecated)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case deprecated:
                    this->GetNovelSystem()->PopComponent(this);
                    break;
                    
                default:
                    break;
            }
        }
        void Character::Draw(sf::RenderWindow* window)
        {
            if (spriteLoaded && visible)
                window->draw(sprite);
        }
        void Character::Destroy()
        {
            if (groupPointer != nullptr && novel != nullptr)
                novel->RemoveFromGroup(groupPointer);
        }
        void Character::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        void Character::SetGroup(List<Character>* element)
        {
            this->groupPointer = element;
        }
        void Character::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
                if (novel != nullptr)
                    if (newMode == disappearing && sendMessageBack == atDisappearing)
                        novel->UnHold(this);
            }
        }
        CharacterData* Character::GetCharacterData()
        {
            return characterData;
        }
        void Character::SetCharacterData(CharacterData* characterData)
        {
            this->characterData = characterData;
        }
        void Character::SetPosition(positionEnum position, float x = 0, float y = 0)
        {
            this->position = position;
            customX = x;
            customY = y;
        }
    }
}
