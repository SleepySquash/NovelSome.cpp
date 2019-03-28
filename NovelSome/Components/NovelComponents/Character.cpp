//
//  Character.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Character.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Character::LoadState(const std::wstring& state)
        {
            spriteLoaded = false;
            if (characterData)
            {
                std::wstring fullPath = folderPath + characterData->filePath;
                if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
                std::wstring lookForSpritePath = base::GetFolderPath(folderPath + characterData->filePath);
                std::wstring spritePath{ L"" };
                scaleRawX = 1.f; scaleRawY = 1.f;
                
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
                    bool eof{ false }; std::wstring line;
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
                                    std::wstring stateName = nss::ParseAsMaybeQuoteString(command);
                                    stateReading = (stateName == state) ? true : false;
                                    if (stateReading)
                                    {
                                        vector<std::wstring> arguments;
                                        nss::ParseArguments(command, arguments);
                                        for (auto arg : arguments)
                                        {
                                            nss::CommandSettings argument; argument.Command(arg);
                                            if (nss::Command(argument, L"normal") || nss::Command(argument, L"n"))
                                                parallaxPower = gs::defaultParallaxNormal;
                                            else if (nss::Command(argument, L"close") || nss::Command(argument, L"c"))
                                                parallaxPower = gs::defaultParallaxClose;
                                            else if (nss::Command(argument, L"far") || nss::Command(argument, L"f"))
                                                parallaxPower = gs::defaultParallaxFar;
                                            else if (nss::Command(argument, L"background") || nss::Command(argument, L"back") || nss::Command(argument, L"b"))
                                                parallaxPower = gs::defaultParallaxBackground;
                                            else if (nss::Command(argument, L"frontground") || nss::Command(argument, L"front") || nss::Command(argument, L"f"))
                                                parallaxPower = gs::defaultParallaxFrontground;
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
                                    scaleRawX = nss::ParseAsFloat(command);
                                    scaleRawY = scaleRawX;
                                }
                                else if (nss::Command(command, L"scalexy "))
                                {
                                    scaleRawX = nss::ParseAsFloat(command);
                                    SkipSpaces(command);
                                    scaleRawY = nss::ParseAsFloat(command);
                                }
                                else if (nss::Command(command, L"scalex "))
                                    scaleRawX = nss::ParseAsFloat(command);
                                else if (nss::Command(command, L"scaley "))
                                    scaleRawY = nss::ParseAsFloat(command);
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
                                    else parallaxPower = base::atof(possibleParallax);
                                }
                                
                                eof = stateReading ? false : true;
                            }
                        } else eof = true;
                    }
                }
                wif.close();
                
                if (spritePath.length() != 0)
                {
                    sf::Texture* texturePtr = ic::RequestHigherTexture(lookForSpritePath + spritePath, novelSystem, 2);
                    if ((spriteLoaded = texturePtr))
                    {
                        imagePath = lookForSpritePath + spritePath;
                        sprite.setTexture(*texturePtr, true);
                        
                        /// scale 1 = scale that makes image fit in height with 800 pixels
                        float scaleFactor = (float)gs::relativeHeight / texturePtr->getSize().y;
                        scaleX = scaleRawX * scaleFactor; scaleY = scaleRawX * scaleFactor;
                        sprite.setScale(scaleX, scaleY);
                        
                        Resize(gs::width, gs::height);
                    }
                }
                
                if (!spriteLoaded)
                {
                    if (sendMessageBack != noMessage) novelSystem->SendMessage({"UnHold", this});
                    novelSystem->PopComponent(this);
                }
            }
            else cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Character::Resize(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                sprite.setScale(scaleX * (doParallax ? (1 + parallaxPower) : 1) * gs::scScale, scaleY * (doParallax ? (1 + parallaxPower) : 1) * gs::scScale);
                sprite.setOrigin(sprite.getLocalBounds().width/2, (sprite.getLocalBounds().height > height/sprite.getScale().y ? height/sprite.getScale().y : (sprite.getLocalBounds().height - (doParallax ? (sprite.getLocalBounds().height*parallaxPower) : 0))));
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
                    if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime)
                    {
                        alpha = maxAlpha; currentTime = 0.f;
                        mode = afterAppearSwitchTo;
                        if (sendMessageBack == atAppearance) novelSystem->SendMessage({"UnHold", this});
                    }
                    else alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case disappearing: gs::requestWindowRefresh = true;
                    if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0; currentTime = 0.f;
                        mode = deprecated;
                        if (sendMessageBack == atDeprecated) novelSystem->SendMessage({"UnHold", this});
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                case deprecated: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                default: break;
            }
        }
        void Character::Draw(sf::RenderWindow* window) { if (spriteLoaded && visible) window->draw(sprite); }
        void Character::Destroy()
        {
            if (imagePath.length() != 0) ic::DeleteImage(imagePath);
            novelSystem->SendMessage({"Destroy", this});
        }
        void Character::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseMoved && mode != deprecated && visible && doParallax && parallaxPower > 0)
                CalculateParallax(event.mouseMove.x, event.mouseMove.y);
        }
        void Character::ReceiveMessage(MessageHolder &message)
        {
            if (nss::Command(message.info, "Request") && message.additional == imagePath)
            {
                sf::Texture* texture = ic::LoadTexture(imagePath, 2);
                if (texture) {
                    sprite.setTexture(*texture, true);
                    /// scale 1 = scale that makes image fit in height with 800 pixels
                    float scaleFactor = (float)gs::relativeHeight / texture->getSize().y;
                    scaleX = scaleRawX * scaleFactor; scaleY = scaleRawX * scaleFactor;
                    sprite.setScale(scaleX, scaleY);
                    Resize(gs::width, gs::height);
                }
            }
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
        void Character::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode && mode != deprecated)
            {
                currentTime = 0.f; mode = newMode;
                if (newMode == disappearing && sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
            }
        }
        void Character::SetPosition(positionEnum pos, float x = 0, float y = 0) { position = pos; customX = x; customY = y; }
    }
}
