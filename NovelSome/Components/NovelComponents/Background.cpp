//
//  Background.cpp
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
        void Background::LoadImage(const sf::String& path)
        {
            spriteLoaded = false;
            backgroundLoaded = false;
            if (novel != nullptr)
            {
                sf::Texture* texturePtr = ic::LoadTexture(novel->GetFolderPath() + path, 1);
                if (texturePtr != nullptr)
                {
                    imagePath = novel->GetFolderPath() + sf::String(path);
                    spriteLoaded = true;
                    sprite.setTexture(*texturePtr);
                    
                    Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
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
        void Background::Resize(unsigned int width, unsigned int height)
        {
            CalculateScale(width, height);
            if (doParallax && !(gs::isPauseEnabled && gs::isPause))
                CalculateParallax(sf::Mouse::getPosition(*ns::gs::window).x, sf::Mouse::getPosition(*ns::gs::window).y);
        }
        void Background::Update(const sf::Time& elapsedTime)
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
                default: break;
            }
        }
        void Background::Draw(sf::RenderWindow* window) { if (spriteLoaded && visible) window->draw(sprite); }
        void Background::Destroy()
        {
            if (imagePath.toWideString().length() != 0)
                ic::DeleteImage(imagePath);
            if (novel != nullptr) novel->RemoveFromGroup(groupPointer);
        }
        void Background::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseMoved && mode != deprecated && visible && doParallax && parallaxPower > 0)
                CalculateParallax(event.mouseMove.x, event.mouseMove.y);
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
        void Background::SetNovel(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
            {
                appearTime = novel->skin.background.appearTime;
                disappearTime = novel->skin.background.disappearTime;
                maxAlpha = novel->skin.background.maxAlpha;
                doParallax = novel->skin.background.doParallax;
                parallaxPower = novel->skin.background.parallaxPower;
            }
        }
        void Background::SetGroup(const list<Background*>::iterator& element) { this->groupPointer = element; }
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
                currentTime = 0.f;
                mode = newMode;
                if (novel != nullptr)
                    if (newMode == disappearing && sendMessageBack == atDisappearing)
                        novel->UnHold(this);
            }
        }
    }
}
