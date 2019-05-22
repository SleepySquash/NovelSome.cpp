//
//  Background.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Background.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Background::Background(const std::wstring& folderPath) : folderPath(folderPath), Savable(L"Background") { }
        void Background::LoadImage(const std::wstring& path)
        {
            imageName = path;
            sf::Texture* texture = ic::RequestHigherTexture(folderPath + path, novelSystem, 1);
            if ((spriteLoaded = texture))
            {
                imagePath = folderPath + path;
                sprite.setTexture(*texture); Resize(gs::width, gs::height);
            }
            
            if (!spriteLoaded)
            {
                if (sendMessageBack != noMessage) novelSystem->SendMessage({"UnHold", this});
                novelSystem->PopComponent(this);
            }
        }
        void Background::Resize(const unsigned int& width, const unsigned int& height)
        {
            CalculateScale(width, height);
            if (doParallax && !(gs::isPauseEnabled && gs::isPause))
                CalculateParallax(sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
        }
        void Background::Update(const sf::Time& elapsedTime)
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
                    else alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;

                case deprecated: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                default: break;
            }
        }
        void Background::Draw(sf::RenderWindow* window) { if (spriteLoaded && visible) window->draw(sprite); }
        void Background::Destroy() { if (imagePath.length() != 0) ic::DeleteImage(imagePath); novelSystem->SendMessage({"Destroy", this}); }
        void Background::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseMoved && mode != deprecated && visible && doParallax && parallaxPower > 0)
                CalculateParallax(event.mouseMove.x, event.mouseMove.y);
        }
        void Background::ReceiveMessage(MessageHolder &message)
        {
            if (nss::Command(message.info, "Request") && message.additional == imagePath)
            {
                sf::Texture* texture = ic::LoadTexture(imagePath);
                if (texture) { sprite.setTexture(*texture, true); CalculateScale(gs::width, gs::height);
                    if (doParallax && !(gs::isPauseEnabled && gs::isPause))
                        CalculateParallax(sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                }
            }
        }
        void Background::CalculateParallax(int mouseX, int mouseY)
        {
            if (mouseX >= 0 && mouseY >= 0 && mouseX <= gs::width && mouseY <= gs::height)
            {
                float posX = defaultPositionX + (int)(mouseX - gs::width/2) * parallaxPower;
                float posY = defaultPositionY + (int)(mouseY - gs::height/2) * parallaxPower;
                sprite.setPosition(posX, posY);
            }
        }
        void Background::CalculateScale(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                float scaleFactorX, scaleFactorY, scaleFactor;
                scaleFactorX = (float)width / (sprite.getTexture()->getSize().x) * (doParallax? 1 + parallaxPower : 1) * scaleX;
                scaleFactorY = (float)height / (sprite.getTexture()->getSize().y) * (doParallax? 1 + parallaxPower : 1) * scaleY;
                switch (fitMode)
                {
                    case defaultFit:
                        scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                        sprite.setScale(scaleFactor, scaleFactor);
                        break;
                    case fillCentre:
                        scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                        sprite.setScale(scaleFactor, scaleFactor);
                        defaultPositionX = (float)ns::gs::width/2 - sprite.getLocalBounds().width/2*sprite.getScale().x - sprite.getOrigin().x*sprite.getScale().x;
                        defaultPositionY = (float)ns::gs::height/2 - sprite.getLocalBounds().height/2*sprite.getScale().y - sprite.getOrigin().y*sprite.getScale().y;
                        sprite.setPosition(defaultPositionX, defaultPositionY);
                        break;
                    case stretch: sprite.setScale(scaleFactorX, scaleFactorY); break;
                    default: break;
                }
            }
        }
        void Background::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode && mode != deprecated)
            {
                currentTime = 0.f; mode = newMode;
                if (newMode == disappearing && sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
            }
        }
        void Background::Save(std::wofstream& wof)
        {
            if (spriteLoaded)
            {
                wof << L"path: " << imageName << endl;
                if (!visible) wof << L"visible: " << visible << endl;
                if (parallaxPower != gs::defaultParallaxBackground) wof << L"parallaxPower: " << parallaxPower << endl;
                if (maxAlpha != 255) wof << L"maxAlpha: " << maxAlpha << endl;
                
                if (mode != existing)
                {
                    wof << L"mode: " << mode << endl;
                    if (mode == appearing || mode == disappearing) wof << L"currentTime: " << currentTime << endl;
                }
            }
        }
        void Background::Load(std::wifstream& wof)
        {
            cout << "load called" << endl;
        }
    }
}
