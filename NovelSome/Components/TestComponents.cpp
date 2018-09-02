//
//  TestComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "TestComponents.hpp"

namespace ns
{
    namespace TestComponents
    {
        TestComponent::TestComponent()
        {
            text.setString("");
        }
        TestComponent::TestComponent(std::string str)
        {
            text.setString(str);
        }
        void TestComponent::Init()
        {
            text.setCharacterSize(12);
            text.setFillColor(sf::Color::White);
        }
        void TestComponent::Draw(sf::RenderWindow* window)
        {
            window->draw(text);
        }
        void TestComponent::SetString(std::string str)
        {
            text.setString(str);
        }
        void TestComponent::SetFont(const sf::Font& font)
        {
            text.setFont(font);
        }
        void TestComponent::SetSize(unsigned int size)
        {
            text.setCharacterSize(size);
        }
        void TestComponent::SetColor(const sf::Color& color)
        {
            text.setFillColor(color);
        }
        void TestComponent::SetPosition(float x, float y)
        {
            text.setPosition(x, y);
        }
        
        
        
        EntityListComponent::EntityListComponent(ns::EntitySystem system)
        {
            this->system = system;
        }
        void EntityListComponent::Init()
        {
            text.setFont(ns::FontCollector::GetFont("Sansation.ttf"));
            fontLoaded = (text.getFont() != nullptr);
            text.setCharacterSize(50);
            text.setFillColor(sf::Color::Black);
        }
        void EntityListComponent::Draw(sf::RenderWindow* window)
        {
            int yy = 0; int i = 0;
            for (List<ns::Entity>* entity = system.GetEntitiesListHead(); entity != nullptr; entity = entity->next)
            {
                std::string string = "Entity №" + std::to_string(i) + " with Components: " + std::to_string(entity->data->GetComponentsCount());
                text.setPosition(0, yy);
                text.setString(string);
                if (fontLoaded)
                    window->draw(text);
                
                i++;
                yy += text.getLocalBounds().height + 10;
            }
        }
        
        
        
        DebugComponent::DebugComponent() { }
        void DebugComponent::Init()
        {
            buildText.setString("Update 0 build 1");
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
        
        
        
        BackgroundComponent::BackgroundComponent(std::string path)
        {
            if (image.loadFromFile(resourcePath() + "Data/Images/" + path))
            {
                imagePath = path;
                if (texture.loadFromImage(image))
                {
                    spriteLoaded = true;
                    sprite.setTexture(texture);
                }
            }
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
                        mode = existing;
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
        
        
        
        DialogueComponent::DialogueComponent(sf::String dialogue)
        {
            text.setString(dialogue);
        }
        void DialogueComponent::Init()
        {
            text.setFont(ns::FontCollector::GetFont(fontName));
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            shape.setFillColor(sf::Color(0,0,0,150));
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
                        mode = waitingForInput;
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
                    mode = (!mustPressInsideDialogue || (event.mouseButton.x > 0 && event.mouseButton.y < ns::GlobalSettings::width && event.mouseButton.y < ns::GlobalSettings::height && event.mouseButton.y > ns::GlobalSettings::height - ns::GlobalSettings::height/5)) ? disappearing : waitingForInput;
                    
                    //TODO: Clear mouse input
                    event = sf::Event();
                }
        }
        void DialogueComponent::Draw(sf::RenderWindow* window)
        {
            window->draw(shape);
            window->draw(text);
        }
        void DialogueComponent::Resize(unsigned int width, unsigned int height)
        {
            text.setPosition(30, height - height/5 + 10);
            shape.setPosition(0, height - height/5);
            shape.setSize({static_cast<float>(width), static_cast<float>(height/5)});
        }
    }
}
