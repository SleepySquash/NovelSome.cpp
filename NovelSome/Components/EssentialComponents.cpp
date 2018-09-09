//
//  EssentialComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "EssentialComponents.hpp"

namespace ns
{
    namespace EssentialComponents
    {
        DebugComponent::DebugComponent(sf::String build)
        {
            buildText.setString(build);
        }
        void DebugComponent::Init()
        {
            buildText.setFillColor(sf::Color::White);
            buildText.setOutlineColor(sf::Color::Black);
            buildText.setOutlineThickness(1.f);
            buildText.setCharacterSize(16);
            buildText.setFont(ns::FontCollector::GetFont("Pacifica.ttf"));
            fontLoaded = (buildText.getFont() != nullptr);
            
            projectText.setString("NovelSome by SlSq");
            projectText.setFillColor(sf::Color::White);
            projectText.setOutlineColor(sf::Color::Black);
            projectText.setOutlineThickness(1.f);
            projectText.setCharacterSize(24);
            projectText.setFont(ns::FontCollector::GetFont("Pacifica.ttf"));
            
            performanceText.setString("FPS is 999");
            performanceText.setFillColor(sf::Color::White);
            performanceText.setOutlineColor(sf::Color::Black);
            performanceText.setOutlineThickness(1.f);
            performanceText.setCharacterSize(16);
            performanceText.setFont(ns::FontCollector::GetFont("Pacifica.ttf"));
        }
        void DebugComponent::Update(const sf::Time& elapsedTime)
        {
            
        }
        void DebugComponent::Draw(sf::RenderWindow* window)
        {
            if (fontLoaded)
            {
                window->draw(buildText);
                window->draw(projectText);
                window->draw(performanceText);
            }
        }
        void DebugComponent::Resize(unsigned int width, unsigned int height)
        {
            int yy = height - 10;
            buildText.setPosition(width - buildText.getLocalBounds().width - 10, yy - buildText.getLocalBounds().height);
            yy -= buildText.getLocalBounds().height*2 - 10;
            projectText.setPosition(width - projectText.getLocalBounds().width - 10, yy - projectText.getLocalBounds().height);
            yy -= projectText.getLocalBounds().height*2 - 20;
            performanceText.setPosition(width - performanceText.getLocalBounds().width - 10, yy - performanceText.getLocalBounds().height);
        }
        
        
        
        GamePause::GamePause()
        {
            shape.setFillColor(sf::Color(0,0,0,0));
        }
        void GamePause::Update(const sf::Time& elapsedTime)
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
                        mode = waiting;
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    break;
                    
                case disappearing:
                    if (currentTime < disappearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0;
                        currentTime = 0.f;
                        mode = waiting;
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    break;
                    
                default:
                    break;
            }
        }
        void GamePause::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                ns::GlobalSettings::isPause = !ns::GlobalSettings::isPause;
                mode = ns::GlobalSettings::isPause? appearing : disappearing;
                currentTime = 0.f;
            }
        }
        void GamePause::Draw(sf::RenderWindow *window)
        {
            if (alpha != 0)
                window->draw(shape);
        }
        void GamePause::Resize(unsigned int width, unsigned int height)
        {
            shape.setSize({(float)width, (float)height});
        }
    }
}
