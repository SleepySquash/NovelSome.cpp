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
        Character::Character() : Savable(L"Character") { }
        void Character::LoadState(const std::wstring& stateName)
        {
            state = stateName; spriteLoaded = false;
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
                    sf::Texture* texturePtr = ic::RequestHigherTexture(lookForSpritePath + spritePath, /*novelSystem*/ic::globalRequestSender, 2);
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
            else cout << "Error :: Character :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Character::Resize(const unsigned int& width, const unsigned int& height)
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
                    else alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
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
                        alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
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
        void Character::Save(std::wofstream& wof)
        {
            if (spriteLoaded)
            {
                if (characterData) wof << L"name: " << characterData->name << endl;
                if (position != center) wof << L"position: " << position << endl;
                if (state.length() != 0) wof << L"state: " << state << endl;
                
                if (mode != existing)
                {
                    wof << L"mode: " << mode << endl;
                    if (mode == appearing || mode == disappearing) wof << L"currentTime: " << currentTime << endl;
                    if (mode == appearing && ((Skin::self && appearTime != Skin::self->character.appearTime) || (!Skin::self && appearTime != 0.6f))) wof << L"appearTime: " << appearTime << endl;
                }
                if ((Skin::self && disappearTime != Skin::self->character.disappearTime) || (!Skin::self && disappearTime != 0.6f)) wof << L"disappearTime: " << disappearTime << endl;
                if ((Skin::self && maxAlpha != Skin::self->character.maxAlpha) || (!Skin::self && maxAlpha != 255)) wof << L"maxAlpha: " << maxAlpha << endl;
                if (sendMessageBack != atAppearance) wof << L"send: " << sendMessageBack << endl;
            }
        }
        std::pair<std::wstring, bool> Character::Load(std::wifstream& wif)
        {
            mode = existing; std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"name: "))
                {
                    std::wstring name = nss::ParseUntil(command, L'\n');
                    auto it = CharacterLibrary::find(name);
                    if (it != CharacterLibrary::library.end()) characterData = (*it).second;
                }
                else if (nss::Command(command, L"position: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: position = positionEnum::custom; break;
                        case 1: position = positionEnum::left; break;
                        case 2: position = positionEnum::cleft; break;
                        case 4: position = positionEnum::cright; break;
                        case 5: position = positionEnum::right; break;
                        default: position = positionEnum::center; break;
                    }
                }
                else if (nss::Command(command, L"state: ")) LoadState(nss::ParseUntil(command, L'\n'));
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = modeEnum::appearing; break;
                        case 2: mode = modeEnum::disappearing; break;
                        case 3: mode = modeEnum::deprecated; break;
                        default: mode = modeEnum::existing; break;
                    }
                }
                else if (nss::Command(command, L"currenttime: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"appeartime: ")) appearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"disappeartime: ")) disappearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"parallaxpower: ")) parallaxPower = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"visible: ")) visible = nss::ParseAsBool(command);
                else if (nss::Command(command, L"maxalpha: ")) maxAlpha = nss::ParseAsInt(command);
                else if (nss::Command(command, L"send: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: sendMessageBack = sendMessageBackEnum::noMessage; break;
                        case 2: sendMessageBack = sendMessageBackEnum::atDisappearing; break;
                        case 3: sendMessageBack = sendMessageBackEnum::atDeprecated; break;
                        default: sendMessageBack = sendMessageBackEnum::atAppearance; break;
                    }
                }
                if (wif.eof()) done = true;
            }
            
            if (mode == modeEnum::appearing) alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
            else if (mode == modeEnum::disappearing) alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
            else alpha = maxAlpha;
            if (spriteLoaded) sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
            bool onHold{ !((sendMessageBack == sendMessageBackEnum::noMessage) || (sendMessageBack == sendMessageBackEnum::atAppearance && mode > 0) || (sendMessageBack == sendMessageBackEnum::atDisappearing && mode > 1)) };
            
            return { line, onHold };
        }
    }
}
