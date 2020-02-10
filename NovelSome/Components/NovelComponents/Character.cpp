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
            state = stateName; loaded = false;
            if (characterData)
            {
                std::wstring fullPath = folder + characterData->filePath;
                if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
                std::wstring lookForSpritePath = base::GetFolderPath(folder + characterData->filePath);
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
                    if ((loaded = texturePtr))
                    {
                        image = lookForSpritePath + spritePath;
                        sprite.setTexture(*texturePtr, true);
                        
                        /// scale 1 = scale that makes image fit in height with 800 pixels
                        float scaleFactor = (float)gs::relativeHeight / texturePtr->getSize().y;
                        scaleX = scaleRawX * scaleFactor; scaleY = scaleRawX * scaleFactor;
                        sprite.setScale(scaleX, scaleY);
                        
                        Resize(gs::width, gs::height);
                    }
                }
                
                if (!loaded)
                {
                    if (messageBack != MessageBack::No)
                        novelSystem->SendMessage({"UnHold", this});
                    novelSystem->PopComponent(this);
                }
            }
            else cout << "Error :: Character :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Character::Resize(const unsigned int& width, const unsigned int& height)
        {
            if (loaded)
            {
                sprite.setScale(scaleX * (doParallax ? (1 + parallaxPower) : 1) * gs::scScale, scaleY * (doParallax ? (1 + parallaxPower) : 1) * gs::scScale);
                sprite.setOrigin(sprite.getLocalBounds().width/2, (sprite.getLocalBounds().height > height/sprite.getScale().y ? height/sprite.getScale().y : (sprite.getLocalBounds().height - (doParallax ? (sprite.getLocalBounds().height*parallaxPower) : 0))));
                switch (position)
                {
                    case Position::Left: sprite.setPosition((float)width/6, height); break;
                    case Position::CLeft: sprite.setPosition((float)width/3, height); break;
                    case Position::Center: sprite.setPosition((float)width/2, height); break;
                    case Position::CRight: sprite.setPosition((width - (float)width/3), height); break;
                    case Position::Right: sprite.setPosition((width - (float)width/6), height); break;
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
                case Mode::Appear: gs::requestWindowRefresh = true;
                    if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime) { alpha = maxAlpha; currentTime = disappearTime; mode = switchTo;
                        if (messageBack == MessageBack::AtAppearance) novelSystem->SendMessage({"UnHold", this}); }
                    else alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case Mode::Disapper: gs::requestWindowRefresh = true;
                    if (currentTime > 0) currentTime -= elapsedTime.asSeconds();
                    if (currentTime <= 0) { alpha = 0; currentTime = 0.f; mode = Mode::Deprecate;
                        if (messageBack == MessageBack::AtDeprecated) novelSystem->SendMessage({"UnHold", this}); }
                    else alpha = (sf::Uint8)(maxAlpha * (currentTime / disappearTime));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                case Mode::Deprecate: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                default: break;
            }
        }
        void Character::Draw(sf::RenderWindow* window) { if (loaded && visible) window->draw(sprite); }
        void Character::Destroy()
        {
            if (image.length()) ic::DeleteImage(image);
            novelSystem->SendMessage({"Destroy", this});
        }
        void Character::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseMoved && mode != Mode::Deprecate && visible && doParallax && parallaxPower > 0)
                CalculateParallax(event.mouseMove.x, event.mouseMove.y);
        }
        void Character::ReceiveMessage(MessageHolder &message)
        {
            if (nss::Command(message.info, "Request") && message.additional == image)
            {
                sf::Texture* texture = ic::LoadTexture(image, 2);
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
        void Character::SetStateMode(const Mode& newMode)
        {
            if (mode != newMode && mode != Mode::Deprecate)
            {
                if (newMode == Mode::Disapper) currentTime = disappearTime; else currentTime = 0.f; mode = newMode;
                if ((newMode == Mode::Disapper && messageBack == MessageBack::AtDisappearance) ||
                    (newMode == Mode::Deprecate && messageBack == MessageBack::AtDeprecated))
                    novelSystem->SendMessage({"UnHold", this});
            }
        }
        void Character::SetPosition(const Position& pos, float x, float y) { position = pos; customX = x; customY = y; }
        void Character::Save(std::wofstream& wof)
        {
            if (loaded)
            {
                if (characterData) wof << L"name: " << characterData->name << endl;
                if (position != Position::Center) wof << L"position: " << (int)position << endl;
                if (state.length() != 0) wof << L"state: " << state << endl;
                
                if (mode != Mode::Exist)
                {
                    wof << L"mode: " << (int)mode << endl;
                    if (mode == Mode::Appear || mode == Mode::Disapper) wof << L"currentTime: " << currentTime << endl;
                    if (mode == Mode::Appear && ((Skin::self && appearTime != Skin::self->character.appearTime) || (!Skin::self && appearTime != 0.6f))) wof << L"appearTime: " << appearTime << endl;
                }
                if ((Skin::self && disappearTime != Skin::self->character.disappearTime) || (!Skin::self && disappearTime != 0.6f)) wof << L"disappearTime: " << disappearTime << endl;
                if ((Skin::self && maxAlpha != Skin::self->character.maxAlpha) || (!Skin::self && maxAlpha != 255)) wof << L"maxAlpha: " << maxAlpha << endl;
                if (messageBack != MessageBack::AtAppearance) wof << L"send: " << (int)messageBack << endl;
            }
        }
        std::pair<std::wstring, bool> Character::Load(std::wifstream& wif)
        {
            mode = Mode::Exist; std::wstring line; nss::CommandSettings command; bool done{ false };
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
                        case 0: position = Position::Custom; break;
                        case 1: position = Position::Left; break;
                        case 2: position = Position::CLeft; break;
                        case 4: position = Position::CRight; break;
                        case 5: position = Position::Right; break;
                        default: position = Position::Center; break;
                    }
                }
                else if (nss::Command(command, L"state: ")) LoadState(nss::ParseUntil(command, L'\n'));
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = Mode::Appear; break;
                        case 2: mode = Mode::Disapper; break;
                        case 3: mode = Mode::Deprecate; break;
                        default: mode = Mode::Exist; break;
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
                        case 0: messageBack = MessageBack::No; break;
                        case 2: messageBack = MessageBack::AtDisappearance; break;
                        case 3: messageBack = MessageBack::AtDeprecated; break;
                        default: messageBack = MessageBack::AtAppearance; break;
                    }
                }
                if (wif.eof()) done = true;
            }
            
            if (mode == Mode::Appear) alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
            else if (mode == Mode::Disapper) alpha = (sf::Uint8)(maxAlpha * (currentTime / disappearTime));
            else alpha = maxAlpha;
            if (loaded) sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
            bool onHold{ !((messageBack == MessageBack::No) || (messageBack == MessageBack::AtAppearance && (int)mode > 0) || (messageBack == MessageBack::AtDisappearance && (int)mode > 1)) };
            
            return { line, onHold };
        }
    }
}
