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
        Dialogue::Dialogue(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
            {
                guiSystem = &(novel->skin.dialogue.gui);
                skin = &(novel->skin.dialogue);
                
                appearTime = novel->skin.dialogue.appearTime;
                disappearTime = novel->skin.dialogue.disappearTime;
                maxAlpha = novel->skin.dialogue.maxAlpha;
                characterSize = novel->skin.dialogue.characterSize;
                fontName = novel->skin.dialogue.fontName;
                
                forcePressInsideDialogue = novel->skin.dialogue.forcePressInsideDialogue && ns::gs::forcePressInsideDialogue;
            }
            if (guiSystem != nullptr)
                guiSystem->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
        }
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
                        guiSystem->SetAlphaIfBigger(alpha);
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    if (drawCharacterName)
                    {
                        charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                        if (charText.getOutlineThickness() != 0)
                            charText.setOutlineColor(sf::Color(charText.getOutlineColor().r, charText.getOutlineColor().g, charText.getOutlineColor().b, alpha));
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
                    if (drawCharacterName)
                    {
                        charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                        if (charText.getOutlineThickness() != 0)
                            charText.setOutlineColor(sf::Color(charText.getOutlineColor().r, charText.getOutlineColor().g, charText.getOutlineColor().b, alpha));
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
            {
                if (visible && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
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
                else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                    visible = !visible;
            }
        }
        void Dialogue::Draw(sf::RenderWindow* window)
        {
            if (visible)
            {
                if (guiSystem != nullptr)
                    guiSystem->Draw(window);
                //window->draw(shape);
                if (fontLoaded)
                {
                    window->draw(text);
                    if (drawCharacterName)
                    {
                        //window->draw(charShape);
                        window->draw(charText);
                    }
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
                if (charString != "")
                    nss::SetStringWithLineBreaks(charText, charString, width - (unsigned int)(skin->nameTextWidth*gs::scale));
                if (textString != "")
                    nss::SetStringWithLineBreaks(text, textString, width - (unsigned int)(skin->textWidth*gs::scale));
            }
            
            if (guiSystem != nullptr)
                guiSystem->Resize(width, height);
            text.setPosition(skin->textXOffset*gs::scale, height - height/5 + skin->textYOffset*gs::scale);
            
            if (drawCharacterName)
            {
                if (character != nullptr && character->outlineThickness != 0)
                    charText.setOutlineThickness(character->outlineThickness * ns::gs::scale);
                charText.setPosition(skin->nameTextXOffset*gs::scale, height - height/5 - skin->nameTextYOffset*gs::scale - charText.getLocalBounds().height);
            }
        }
        void Dialogue::SetCharacter(CharacterData* character)
        {
            if (character != nullptr)
            {
                this->character = character;
                charText.setOutlineThickness(character->outlineThickness);
                charText.setFillColor(sf::Color(character->fillColor.r, character->fillColor.g, character->fillColor.b, alpha));
                charText.setOutlineColor(sf::Color(character->outlineColor.r, character->outlineColor.g, character->outlineColor.b, alpha));
            }
        }
        void Dialogue::SetCharacterName(const sf::String& characterName)
        {
            charString = characterName;
            drawCharacterName = true;
            
            charText.setString(charString);
            charText.setFont(*ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            
            charText.setCharacterSize(characterSize);
            charText.setFillColor(sf::Color::White);
            
            charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
            if (charText.getOutlineThickness() != 0)
                charText.setOutlineColor(sf::Color(charText.getOutlineColor().r, charText.getOutlineColor().g, charText.getOutlineColor().b, alpha));
        }
        void Dialogue::SetGroup(List<Dialogue>* element)
        {
            this->groupPointer = element;
        }
        void Dialogue::SetDialogue(const sf::String& dialogue)
        {
            textString = dialogue;
            text.setString(textString);
            text.setFont(*ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            
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
        void Dialogue::SetGUISystem(GUISystem* system)
        {
            this->guiSystem = system;
        }
    }
}
