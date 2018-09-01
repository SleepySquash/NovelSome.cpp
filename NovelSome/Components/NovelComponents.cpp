//
//  NovelComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "NovelComponents.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void BackgroundComponent::LoadImage(sf::String path)
        {
            spriteLoaded = false;
            if (novel != nullptr)
            {
                if (image.loadFromFile(resourcePath() + novel->GetFolderPath().toAnsiString() + path.toAnsiString()))
                {
                    imagePath = path;
                    if (texture.loadFromImage(image))
                    {
                        spriteLoaded = true;
                        texture.setSmooth(true);
                        sprite.setTexture(texture);
                        
                        Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
                    }
                }
                
                if (!spriteLoaded)
                    if (sendMessageBack != noMessage)
                        novel->UnHold();
            }
            else
                cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void BackgroundComponent::Resize(unsigned int width, unsigned int height)
        {
            CalculateScale(width, height);
        }
        void BackgroundComponent::Update(const sf::Time& elapsedTime)
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
                                novel->UnHold();
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
                                novel->UnHold();
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / appearTime)));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case deprecated:
                    this->GetEntity()->PopComponent(this);
                    break;
                    
                default:
                    break;
            }
        }
        void BackgroundComponent::Draw(sf::RenderWindow* window)
        {
            if (spriteLoaded && visible)
                window->draw(sprite);
        }
        void BackgroundComponent::SetNovel(NovelComponent* novel)
        {
            this->novel = novel;
        }
        void BackgroundComponent::CalculateScale(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                float scaleFactorX = (float)width / image.getSize().x;
                float scaleFactorY = (float)height / image.getSize().y;
                float scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                sprite.setScale(scaleFactor, scaleFactor);
            }
        }
        
        
        
        void DialogueComponent::Update(const sf::Time& elapsedTime)
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
                                novel->UnHold();
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
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
                                novel->UnHold();
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    break;
                    
                case deprecated:
                    this->GetEntity()->PopComponent(this);
                    break;
                    
                case waitingForTime:
                    if (currentTime < waitingTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= waitingTime)
                    {
                        currentTime = 0.f;
                        mode = disappearing;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDisappearing)
                                novel->UnHold();
                    }
                    break;
                    
                default:
                    break;
            }
        }
        void DialogueComponent::PollEvent(sf::Event& event)
        {
            if (mode == waitingForInput)
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    mode = (!forcePressInsideDialogue || (event.mouseButton.x > 0 && event.mouseButton.x < ns::GlobalSettings::width && event.mouseButton.y < ns::GlobalSettings::height && event.mouseButton.y > ns::GlobalSettings::height - ns::GlobalSettings::height/5)) ? disappearing : waitingForInput;
                    
                    if (mode == disappearing)
                    {
                        event = sf::Event();
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDisappearing)
                                novel->UnHold();
                    }
                }
        }
        void DialogueComponent::Draw(sf::RenderWindow* window)
        {
            window->draw(shape);
			if (fontLoaded)
				window->draw(text);
        }
        void DialogueComponent::Resize(unsigned int width, unsigned int height)
        {
            text.setPosition(30, height - height/5 + 10);
            shape.setPosition(0, height - height/5);
            shape.setSize({static_cast<float>(width), static_cast<float>(height/5)});
        }
        void DialogueComponent::SetNovel(NovelComponent* novel)
        {
            this->novel = novel;
        }
        void DialogueComponent::SetDialogue(sf::String dialogue)
        {
            text.setString(dialogue);
            text.setFont(ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            shape.setFillColor(sf::Color(0,0,0,150));
        }
    }
}
