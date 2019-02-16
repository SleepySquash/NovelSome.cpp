//
//  Character.cpp
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
        void Character::LoadState(sf::String state)
        {
            spriteLoaded = false;
            if (novel != nullptr && characterData != nullptr)
            {
                std::wstring fullPath = novel->GetFolderPath() + characterData->filePath;
                if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
                std::wstring lookForSpritePath = base::GetFolderPath(novel->GetFolderPath() + characterData->filePath);
                std::wstring spritePath{ L"" };
                
                scaleX = 1.f;
                scaleY = 1.f;
                
                std::wifstream wif;
#ifdef _WIN32
                wif.open(fullPath);
#else
                wif.open(base::utf8(fullPath));
#endif
                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                
                if (!wif.is_open())
                    cout << "Warning :: Character :: File couldn't be opened, path: " << base::utf8(fullPath) << endl;
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
                                    
                                    if (stateReading)
                                    {
                                        vector<std::wstring> arguments;
                                        nss::ParseArguments(command, arguments);
                                        for (auto arg : arguments)
                                        {
                                            nss::CommandSettings argument;
                                            argument.Command(arg);
                                            
                                            if (nss::Command(argument, L"normal") || nss::Command(argument, L"n"))
                                                parallaxPower = ns::GlobalSettings::defaultParallaxNormal;
                                            else if (nss::Command(argument, L"close") || nss::Command(argument, L"c"))
                                                parallaxPower = ns::GlobalSettings::defaultParallaxClose;
                                            else if (nss::Command(argument, L"far") || nss::Command(argument, L"f"))
                                                parallaxPower = ns::GlobalSettings::defaultParallaxFar;
                                            else if (nss::Command(argument, L"background") || nss::Command(argument, L"back") || nss::Command(argument, L"b"))
                                                parallaxPower = ns::GlobalSettings::defaultParallaxBackground;
                                            else if (nss::Command(argument, L"frontground") || nss::Command(argument, L"front") || nss::Command(argument, L"f"))
                                                parallaxPower = ns::GlobalSettings::defaultParallaxFrontground;
                                        }
                                    }
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
                                    scaleX = nss::ParseAsFloat(command);
                                    scaleY = scaleX;
                                }
                                else if (nss::Command(command, L"scalexy "))
                                {
                                    scaleX = nss::ParseAsFloat(command);
                                    SkipSpaces(command);
                                    scaleY = nss::ParseAsFloat(command);
                                }
                                else if (nss::Command(command, L"scalex "))
                                    scaleX = nss::ParseAsFloat(command);
                                else if (nss::Command(command, L"scaley "))
                                    scaleY = nss::ParseAsFloat(command);
                                else if (nss::Command(command, L"parallax "))
                                {
                                    std::wstring possibleParallax = nss::ArgumentAsString(command);
                                    if (possibleParallax == L"normal" || possibleParallax == L"n")
                                        parallaxPower = ns::GlobalSettings::defaultParallaxNormal;
                                    else if (possibleParallax == L"close" || possibleParallax == L"c")
                                        parallaxPower = ns::GlobalSettings::defaultParallaxClose;
                                    else if (possibleParallax == L"far" || possibleParallax == L"f")
                                        parallaxPower = ns::GlobalSettings::defaultParallaxFar;
                                    else if (possibleParallax == L"background" || possibleParallax == L"back" || possibleParallax == L"b")
                                        parallaxPower = ns::GlobalSettings::defaultParallaxBackground;
                                    else if (possibleParallax == L"frontground" || possibleParallax == L"front" || possibleParallax == L"f")
                                        parallaxPower = ns::GlobalSettings::defaultParallaxFrontground;
                                    else
                                        parallaxPower = base::atof(possibleParallax);
                                }
                                
                                eof = stateReading ? false : true;
                            }
                        }
                        else
                            eof = true;
                    }
                }
                wif.close();
                
                if (spritePath.length() != 0)
                {
                    sf::Texture* texturePtr = ic::LoadTexture(lookForSpritePath + spritePath, 2);
                    if (texturePtr != nullptr)
                    {
                        imagePath = lookForSpritePath + spritePath;
                        
                        spriteLoaded = true;
                        sprite.setTexture(*texturePtr);
                        
                        //scale 1 = scale that makes image fit in height with 800 pixels
                        float scaleFactor = (float)ns::gs::relativeHeight / texturePtr->getSize().y;
                        scaleX *= scaleFactor;
                        scaleY *= scaleFactor;
                        sprite.setScale(scaleX, scaleY);
                        
                        Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
                    }
                }
                
                if (!spriteLoaded)
                {
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
                    this->GetNovelSystem()->PopComponent(this);
                }
            }
            else
                cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Character::Resize(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                sprite.setScale(scaleX * (doParallax ? (1 + parallaxPower) : 1) * gs::scScale, scaleY * (doParallax ? (1 + parallaxPower) : 1) * gs::scScale);
                sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height > height/sprite.getScale().y ? height/sprite.getScale().y : sprite.getLocalBounds().height);
                switch (position)
                {
                    case left: sprite.setPosition((float)width/6, height); break;
                    case cleft: sprite.setPosition((float)width/3, height); break;
                    case center: sprite.setPosition((float)width/2, height); break;
                    case cright: sprite.setPosition((width - (float)width/3), height); break;
                    case right: sprite.setPosition((width - (float)width/6), height); break;
                    default: sprite.setPosition(customX, customY); break;
                }
                
                defaultPositionX = sprite.getPosition().x;
                defaultPositionY = sprite.getPosition().y;
                if (doParallax && !(gs::isPauseEnabled && gs::isPause))
                    CalculateParallax(sf::Mouse::getPosition(*ns::gs::window).x, sf::Mouse::getPosition(*ns::gs::window).y);
            }
        }
        void Character::Update(const sf::Time& elapsedTime)
        {
            switch (mode)
            {
                case appearing: gs::requestWindowRefresh = true;
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
                    
                case disappearing: gs::requestWindowRefresh = true;
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
                    
                case deprecated: gs::requestWindowRefresh = true; this->GetNovelSystem()->PopComponent(this); break;
                    
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
            if (imagePath.toWideString().length() != 0)
                ic::DeleteImage(imagePath);
            if (novel != nullptr) novel->RemoveFromGroup(groupPointer);
        }
        void Character::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseMoved && mode != deprecated && visible && doParallax && parallaxPower > 0)
                CalculateParallax(event.mouseMove.x, event.mouseMove.y);
        }
        void Character::CalculateParallax(int mouseX, int mouseY)
        {
            if (mouseX >= 0 && mouseY >= 0 && mouseX <= ns::gs::width && mouseY <= ns::gs::height)
            {
                float posX = defaultPositionX + (int)(mouseX - ns::gs::width/2) * parallaxPower;
                float posY = defaultPositionY + (int)(mouseY - ns::gs::height/2) * parallaxPower;
                sprite.setPosition(posX, posY);
            }
        }
        void Character::SetNovel(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
            {
                appearTime = novel->skin.character.appearTime;
                disappearTime = novel->skin.character.disappearTime;
                maxAlpha = novel->skin.character.maxAlpha;
                doParallax = novel->skin.character.doParallax;
                parallaxPower = novel->skin.character.parallaxNormal;
            }
        }
        void Character::SetGroup(const list<Character*>::iterator& element)
        {
            this->groupPointer = element;
        }
        void Character::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode && mode != deprecated)
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
