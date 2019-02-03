//
//  Dialogue.cpp
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
                characterInSecond = novel->skin.dialogue.characterInSecond;
                fontName = novel->skin.dialogue.fontName;
                
                leftSpeechAddition = novel->skin.dialogue.leftSpeechAddition;
                rightSpeechAddition = novel->skin.dialogue.rightSpeechAddition;
                afterRedLineShift = novel->skin.dialogue.afterRedLineShift;
                
                switch (novel->skin.dialogue.textAppearMode)
                {
                    case 0:
                        textAppearMode = textAppearModeEnum::fading;
                        break;
                    case 1:
                        textAppearMode = textAppearModeEnum::printing;
                        break;
                    default:
                        break;
                }
                
                forcePressInsideDialogue = novel->skin.dialogue.forcePressInsideDialogue && ns::gs::forcePressInsideDialogue;
            }
        }
        void Dialogue::Update(const sf::Time& elapsedTime)
        {
            if (guiSystem != nullptr)
                guiSystem->Update(elapsedTime);
            
            if (mode != deprecated && textAppearMode == textAppearModeEnum::printing)
                if (textAppearPos < textAppearMax)
                {
                    elapsedCharacterSum += elapsedTime.asSeconds();
                    while (elapsedCharacterSum > characterInSecond && textAppearPos < textAppearMax)
                    {
                        ++textAppearPos;
                        elapsedCharacterSum -= characterInSecond;
                        
                        while (printingString[textAppearI] == L'\n' || printingString[textAppearI] == L' ')
                            currentString += printingString[textAppearI++];
                        currentString += printingString[textAppearI++];
                        
                        text.setString(currentString);
                    }
                }
            
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
                    if (text.getOutlineThickness() != 0)
                        text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, alpha));
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
                    if (text.getOutlineThickness() != 0)
                        text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, alpha));
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
                    
                case waitingForChoose:
                    if (novel == nullptr || novel->chooseGroup.size() == 0) mode = disappearing;
                    break;
                    
                default:
                    break;
            }
        }
        void Dialogue::PollEvent(sf::Event& event)
        {
            if (mode == waitingForInput)
            {
                if (visible && (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::TouchEnded))
                {
                    bool pressed{ false };
                    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                        pressed = (!forcePressInsideDialogue || (event.mouseButton.x > workingArea_l && event.mouseButton.x < workingArea_r && event.mouseButton.y < workingArea_b && event.mouseButton.y > workingArea_t));
                    else if (event.type == sf::Event::TouchEnded)
                        pressed = (!forcePressInsideDialogue || (event.touch.x > workingArea_l && event.touch.x < workingArea_r && event.touch.y < workingArea_b && event.touch.y > workingArea_t));
                    
                    if (pressed)
                    {
                        bool fadeAway{ true };
                        if (textAppearMode == textAppearModeEnum::printing && textAppearPos != textAppearMax)
                            currentString = printingString, text.setString(currentString), textAppearPos = textAppearMax, fadeAway = false;
                        
                        event = sf::Event();
                        if (fadeAway)
                        {
                            mode = disappearing;
                            if (novel != nullptr)
                                if (sendMessageBack == atDisappearing)
                                    novel->UnHold(this);
                        }
                    }
                }
            }
            else if (mode == waitingForChoose)
            {
                if (visible && (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::TouchEnded))
                {
                    if (textAppearMode == textAppearModeEnum::printing && textAppearPos != textAppearMax)
                    {
                        bool pressed{ false };
                        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                            pressed = (!forcePressInsideDialogue || (event.mouseButton.x > workingArea_l && event.mouseButton.x < workingArea_r && event.mouseButton.y < workingArea_b && event.mouseButton.y > workingArea_t));
                        else if (event.type == sf::Event::TouchEnded)
                            pressed = (!forcePressInsideDialogue || (event.touch.x > workingArea_l && event.touch.x < workingArea_r && event.touch.y < workingArea_b && event.touch.y > workingArea_t));
                        
                        if (pressed)
                        {
                            currentString = printingString;
                            text.setString(currentString);
                            textAppearPos = textAppearMax;
                            
                            event = sf::Event();
                        }
                    }
                }
            }
            if ((mode == waitingForChoose || mode == waitingForInput) && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                visible = !visible;
        }
        void Dialogue::Draw(sf::RenderWindow* window)
        {
            if (visible)
            {
                if (guiSystem != nullptr)
                    guiSystem->Draw(window);
                if (fontLoaded)
                {
                    window->draw(text);
                    if (drawCharacterName)
                        window->draw(charText);
                }
            }
        }
        void Dialogue::Destroy()
        {
            if (novel != nullptr) novel->RemoveFromGroup(groupPointer);
        }
        void Dialogue::Resize(unsigned int width, unsigned int height)
        {
            charText.setCharacterSize((unsigned int)(characterSize * gs::scale));
            text.setCharacterSize((unsigned int)(characterSize * gs::scale));
            
            if (fontLoaded)
            {
                if (novel != nullptr)
                    if (novel->skin.dialogue.outlineThickness != 0)
                        text.setOutlineThickness(novel->skin.dialogue.outlineThickness * gs::scale);
                
                if (drawCharacterName && character != nullptr && character->outlineThickness != 0)
                    charText.setOutlineThickness(character->outlineThickness * gs::scale);
                
                if (guiSystem != nullptr)
                {
                    if (charString != "")
                        guiSystem->ResetResize();
                    guiSystem->Resize(width, height);
                }
                
                int textWidth = width;
                int textXOffset = 0;
                int textYOffset = 0;
                
                int nameTextWidth = width;
                int nameTextXOffset = 0;
                int nameTextYOffset = 0;
                
                if (novel != nullptr)
                {
                    GUIObject* textConstrains = novel->skin.dialogue.textConstrains;
                    if (textConstrains == nullptr)
                        textConstrains = novel->skin.dialogue.dialogueRect;
                    
                    if (textConstrains != nullptr)
                    {
                        textWidth = textConstrains->constrains.width;
                        textXOffset = textConstrains->constrains.posX;
                        textYOffset = textConstrains->constrains.posY;
                    }
                    
                    if (novel->skin.dialogue.dialogueRect != nullptr)
                    {
                        workingArea_l = novel->skin.dialogue.dialogueRect->constrains.posX;
                        workingArea_r = workingArea_l + novel->skin.dialogue.dialogueRect->constrains.width;
                        workingArea_t = novel->skin.dialogue.dialogueRect->constrains.posY;
                        workingArea_b = workingArea_t + novel->skin.dialogue.dialogueRect->constrains.height;
                    }
                    else
                    {
                        workingArea_l = 0;
                        workingArea_r = width;
                        workingArea_t = 0;
                        workingArea_b = height;
                    }
                    
                    if (charString != "")
                    {
                        GUIObject* nameConstrains = novel->skin.dialogue.nameConstrains;
                        if (nameConstrains == nullptr)
                            nameConstrains = novel->skin.dialogue.nameRect;
                        
                        if (nameConstrains != nullptr)
                        {
                            nameTextWidth = nameConstrains->constrains.width;
                            nameTextXOffset = nameConstrains->constrains.posX;
                            nameTextYOffset = nameConstrains->constrains.posY;
                        }
                    }
                }
                
                if (charString != "")
                    charText.setString(nss::GetStringWithLineBreaks(charText, charString, nameTextWidth, afterRedLineShift*gs::scale));
                if (textString != L"")
                {
                    if (textAppearMode ==  textAppearModeEnum::printing)
                    {
                        currentString = L""; textAppearI = 0;
                        printingString = nss::GetStringWithLineBreaks(text, textString, textWidth, afterRedLineShift*gs::scale);
                        for (size_t i = 0; i < textAppearPos; ++i)
                        {
                            while (printingString[textAppearI] == L'\n' || printingString[textAppearI] == L' ')
                                currentString += printingString[textAppearI++];
                            currentString += printingString[textAppearI++];
                        }
                        text.setString(currentString);
                    }
                    else
                        text.setString(nss::GetStringWithLineBreaks(text, textString, textWidth, afterRedLineShift*gs::scale));
                }
            
                text.setPosition(textXOffset, textYOffset);
                
                if (drawCharacterName)
                    charText.setPosition(nameTextXOffset, nameTextYOffset);
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
        void Dialogue::SetGroup(const list<Dialogue*>::iterator& element)
        {
            this->groupPointer = element;
        }
        void Dialogue::SetDialogue(const sf::String& dialogue)
        {
            textString = dialogue;
            if (charString.toWideString().length() != 0)
            {
                if (leftSpeechAddition != 0)
                    textString.insert(0, leftSpeechAddition);
                if (rightSpeechAddition != 0)
                    textString += rightSpeechAddition;
            }
            
            printingString = textString;
            if (textAppearMode == textAppearModeEnum::printing)
            {
                currentString = L"", textAppearMax = base::GetLengthWONewLinesAndSpaces(textString);
                text.setString(currentString);
            }
            else
                text.setString(printingString);
            
            text.setFont(*ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            if (novel != nullptr)
            {
                text.setFillColor(sf::Color(novel->skin.dialogue.fillColor.r, novel->skin.dialogue.fillColor.g, novel->skin.dialogue.fillColor.b, alpha));
                if (novel->skin.dialogue.outlineThickness != 0)
                    text.setOutlineColor(sf::Color(novel->skin.dialogue.outlineColor.r, novel->skin.dialogue.outlineColor.g, novel->skin.dialogue.outlineColor.b, alpha));
            }
            
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
        
        
        
        
        
        
        
        Choose::Choose(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
            {
                guiSystem = &(novel->skin.choose.guiChoose);
                skin = &(novel->skin.dialogue);
                
                appearTime = novel->skin.choose.appearTime;
                disappearTime = novel->skin.choose.disappearTime;
                maxAlpha = novel->skin.choose.maxAlpha;
                characterSize = novel->skin.choose.characterSize;
                fontName = novel->skin.choose.fontName;
            }
            if (guiSystem != nullptr)
                guiSystem->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
        }
        void Choose::Update(const sf::Time& elapsedTime)
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
                        mode = waitingForInput;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atAppearance)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    
                    button.setAlpha(alpha);
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
                    
                    button.setAlpha(alpha);
                    break;
                    
                case deprecated:
                    this->GetNovelSystem()->PopComponent(this);
                    break;
                    
                default:
                    break;
            }
        }
        void Choose::PollEvent(sf::Event& event)
        {
            if (mode == waitingForInput)
            {
                if (visible)
                {
                    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::TouchBegan)
                        button.PollEvent(event);
                    else if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded)
                    {
                        int yy = startingYY; bool found{ false };
                        for (int i = 0; i < choices.size() && !found; ++i)
                        {
                            button.setString(choices[i]);
                            button.setPosition(640*gs::scalex, yy*gs::scaley);
                            
                            if ((found = button.PollEvent(event)))
                            {
                                if (novel != nullptr)
                                {
                                    int until = i+1 < choices.size() ? choiceStart[i+1] : actions.size();
                                    for (int j = until - 1; j >= choiceStart[i]; --j)
                                        novel->lines.push_back(actions[j]);
                                }
                                
                                mode = disappearing;
                                if (novel != nullptr)
                                    if (sendMessageBack == atDisappearing)
                                    {
                                        if (novel->dialogueGroup.size() != 0)
                                            for (auto d : novel->dialogueGroup)
                                                if (d->mode == Dialogue::modeEnum::waitingForChoose)
                                                    d->SetStateMode(Dialogue::modeEnum::disappearing);
                                        novel->UnHold(this);
                                    }
                            }
                            
                            yy += button.text.getGlobalBounds().height/gs::scale + 10;
                        }
                    }
                }
                
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                    visible = !visible;
            }
        }
        void Choose::Draw(sf::RenderWindow* window)
        {
            if (visible)
            {
                if (guiSystem != nullptr)
                {
                    guiSystem->SetAlpha(alpha);
                    guiSystem->Draw(window);
                }
                
                int yy = startingYY;
                for (int i = 0; i < choices.size(); ++i)
                {
                    button.setString(choices[i]);
                    button.setPosition(640*gs::scalex, yy*gs::scaley);
                    button.Draw(window);
                    
                    yy += button.text.getGlobalBounds().height/gs::scale + 10;
                }
            }
        }
        void Choose::Destroy()
        {
            if (novel != nullptr) novel->RemoveFromGroup(groupPointer);
        }
        void Choose::Resize(unsigned int width, unsigned int height)
        {
            if (guiSystem != nullptr)
                guiSystem->Resize(width, height);
            
            button.Resize(width, height);
        }
        void Choose::SetGroup(const list<Choose*>::iterator& element)
        {
            this->groupPointer = element;
        }
        void Choose::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
            }
        }
        void Choose::SetGUISystem(GUISystem* system)
        {
            this->guiSystem = system;
        }
        void Choose::AddChoice(const std::wstring& line)
        {
            choices.push_back(line);
            choiceStart.push_back(actions.size());
        }
        void Choose::AddAction(const std::wstring& line)
        {
            actions.push_back(line);
        }
        void Choose::InitChoose()
        {
            /*cout << "   choices: " << endl;
            for (int i = 0; i < choices.size; ++i)
                cout << ns::base::ConvertToUTF8(choices[i]) << endl;
            
            cout << "   choicesStart: " << endl;
            for (int i = 0; i < choiceStart.size; ++i)
                cout << choiceStart[i] << endl;
            
            cout << "   actions: " << endl;
            for (int i = 0; i < actions.size; ++i)
                cout << ns::base::ConvertToUTF8(actions[i]) << endl;*/
            
            if (choices.size() == 0)
            {
                if (novel != nullptr)
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
                this->GetNovelSystem()->PopComponent(this);
            }
            else
            {
                button.setFont(fontName);
                button.characterSize = characterSize;
                button.ignoreWasPressed = true;
                button.halign = GUI::TextButton::halignEnum::center;
                button.valign = GUI::TextButton::valignEnum::top;
                
                int yy = 0;
                for (int i = 0; i < choices.size(); ++i)
                {
                    button.setString(choices[i]);
                    yy += button.text.getGlobalBounds().height + 10;
                }
                startingYY = 400 - yy/2;
                
                Resize(gs::width, gs::height);
            }
        }
    }
}
