//
//  Dialogue.cpp
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
        void Dialogue::Update(const sf::Time& elapsedTime)
        {
            if (guiSystem != nullptr)
                guiSystem->Update(elapsedTime);
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
                    
                    if (guiSystem != nullptr)
                        guiSystem->SetAlpha(alpha);
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    if (drawCharacterName)
                    {
                        charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                        charShape.setFillColor(sf::Color(charShape.getFillColor().r, charShape.getFillColor().g, charShape.getFillColor().b, alpha));
                    }
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
                    
                    if (guiSystem != nullptr)
                        guiSystem->SetAlpha(alpha);
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    if (drawCharacterName)
                    {
                        charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                        charShape.setFillColor(sf::Color(charShape.getFillColor().r, charShape.getFillColor().g, charShape.getFillColor().b, alpha));
                    }
                    break;
                    
                case deprecated:
                    this->GetNovelSystem()->PopComponent(this);
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
                                novel->UnHold(this);
                    }
                    break;
                    
                default:
                    break;
            }
        }
        void Dialogue::PollEvent(sf::Event& event)
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
                                novel->UnHold(this);
                    }
                }
        }
        void Dialogue::Draw(sf::RenderWindow* window)
        {
            if (guiSystem != nullptr)
                guiSystem->Draw(window);
            window->draw(shape);
            if (fontLoaded)
            {
                window->draw(text);
                if (drawCharacterName)
                {
                    window->draw(charShape);
                    window->draw(charText);
                }
            }
        }
        void Dialogue::Destroy()
        {
            if (groupPointer != nullptr && novel != nullptr)
                novel->RemoveFromGroup(groupPointer);
        }
        void Dialogue::Resize(unsigned int width, unsigned int height)
        {
            charText.setCharacterSize((unsigned int)(characterSize * gs::scale));
            text.setCharacterSize((unsigned int)(characterSize * gs::scale));
            
            if (fontLoaded)
            {
                nss::SetStringWithLineBreaks(charText, charString, width - (unsigned int)(35*gs::scale*2));
                nss::SetStringWithLineBreaks(text, textString, width - (unsigned int)(30*gs::scale*2));
            }
            
            if (guiSystem != nullptr)
                guiSystem->Resize(width, height);
            text.setPosition(30*gs::scale, height - height/5 + 10*gs::scale);
            shape.setPosition(0, height - height/5);
            shape.setSize({static_cast<float>(width), static_cast<float>(height/5)});
            
            if (drawCharacterName)
            {
                charText.setPosition(35*gs::scale, height - height/5 - 20*gs::scale - charText.getLocalBounds().height);
                charShape.setPosition(30*gs::scale, height - height/5 - 15*gs::scale - charText.getLocalBounds().height);
                charShape.setSize({static_cast<float>(charText.getLocalBounds().width + 15*gs::scale), static_cast<float>(charText.getLocalBounds().height + 10*gs::scale)});
            }
        }
        void Dialogue::SetNovel(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
                guiSystem = &(novel->dialogueGUI);
            if (guiSystem != nullptr)
                guiSystem->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
        }
        void Dialogue::SetCharacter(sf::String character)
        {
            charString = character;
            drawCharacterName = true;
            
            charText.setString(charString);
            charText.setFont(*ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            
            charText.setCharacterSize(characterSize);
            charText.setFillColor(sf::Color::White);
            charShape.setFillColor(sf::Color(0,0,0,150));
            
            charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
            charShape.setFillColor(sf::Color(charShape.getFillColor().r, charShape.getFillColor().g, charShape.getFillColor().b, alpha));
        }
        void Dialogue::SetGroup(List<Dialogue>* element)
        {
            this->groupPointer = element;
        }
        void Dialogue::SetDialogue(sf::String dialogue)
        {
            textString = dialogue;
            text.setString(textString);
            text.setFont(*ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            shape.setFillColor(sf::Color(0,0,0,150));
            
            Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
        }
        void Dialogue::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
            }
        }
    }
}
