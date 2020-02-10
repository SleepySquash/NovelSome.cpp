//
//  Dialogue.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Dialogue.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Dialogue::Dialogue(GUISystem* guiSystem) : guiSystem(guiSystem), Savable(L"Dialogue") { }
        void Dialogue::Init()
        {
            if (Skin::self)
            {
                appearTime = Skin::self->dialogue.appearTime;
                disappearTime = Skin::self->dialogue.disappearTime;
                maxAlpha = Skin::self->dialogue.maxAlpha;
                characterSize = Skin::self->dialogue.characterSize;
                characterInSecond = Skin::self->dialogue.characterInSecond;
                fontName = Skin::self->dialogue.fontName;
                
                leftSpeechAddition = Skin::self->dialogue.leftSpeechAddition;
                rightSpeechAddition = Skin::self->dialogue.rightSpeechAddition;
                afterRedLineShift = Skin::self->dialogue.afterRedLineShift;
                
                switch (Skin::self->dialogue.textAppearMode)
                {
                    case 0: textAppearMode = textAppearModeEnum::fading; break;
                    case 1: textAppearMode = textAppearModeEnum::printing; break;
                    default: break;
                }
                
                forcePressInsideDialogue = Skin::self->dialogue.forcePressInsideDialogue && gs::forcePressInsideDialogue;
            }
        }
        void Dialogue::Update(const sf::Time& elapsedTime)
        {
            if (guiSystem) guiSystem->Update(elapsedTime);
            if (mode != deprecated && visible && textAppearMode == textAppearModeEnum::printing && textAppearPos < textAppearMax)
            {
                elapsedCharacterSum += elapsedTime.asSeconds();
                while (elapsedCharacterSum > characterInSecond && textAppearPos < textAppearMax)
                {
                    gs::requestWindowRefresh = true;
                    ++textAppearPos; elapsedCharacterSum -= characterInSecond;
                    
                    while (printingString[textAppearI] == L'\n' || printingString[textAppearI] == L' ')
                        currentString += printingString[textAppearI++];
                    currentString += printingString[textAppearI++];
                    text.setString(currentString);
                }
            }
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
                    UpdateAlpha(true);
                    break;
                    
                case disappearing: gs::requestWindowRefresh = true;
                    if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0; currentTime = 0.f; mode = deprecated;
                        if (sendMessageBack == atDeprecated) novelSystem->SendMessage({"UnHold", this});
                    }
                    else alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    UpdateAlpha();
                    break;
                case deprecated: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                case waitingForTime:
                    if (currentTime < waitingTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= waitingTime)
                    {
                        currentTime = 0.f; mode = disappearing;
                        if (sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
                    }
                    break;
                case waitingForChoose: /*if (novel == nullptr || novel->chooseGroup.size() == 0) mode = disappearing;*/ break;
                default: break;
            }
        }
        void Dialogue::PollEvent(sf::Event& event)
        {
            if (mode == waitingForChoose || mode == waitingForInput)
            {
                if (guiSystem && visible) guiSystem->PollEvent(event);
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) visible = !visible;
            }
            if (mode == waitingForInput && visible)
            {
                if (wasPressed && (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded))
                {
                    wasPressed = workingArea.contains(gs::lastMousePos.first, gs::lastMousePos.second);
                    if (wasPressed)
                    {
                        bool fadeAway{ true }; wasPressed = false;
                        if (textAppearMode == textAppearModeEnum::printing && textAppearPos != textAppearMax)
                            currentString = printingString, text.setString(currentString), textAppearPos = textAppearMax, fadeAway = false;
                        
                        event = sf::Event();
                        if (fadeAway)
                        {
                            mode = disappearing;
                            if (sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::TouchBegan)
                    wasPressed = workingArea.contains(gs::lastMousePos.first, gs::lastMousePos.second);
            }
            else if (mode == waitingForChoose)
            {
                if (visible && (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded) && textAppearMode == textAppearModeEnum::printing && textAppearPos != textAppearMax)
                {
                    bool pressed{ ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left && workingArea.contains(event.mouseButton.x, event.mouseButton.y)) || (event.type == sf::Event::TouchEnded && workingArea.contains(event.touch.x, event.touch.y))) };
                    if (pressed)
                    {
                        currentString = printingString; text.setString(currentString);
                        textAppearPos = textAppearMax; event = sf::Event();
                    }
                }
            }
        }
        void Dialogue::Draw(sf::RenderWindow* window)
        {
            if (visible)
            {
                if (guiSystem) guiSystem->Draw(window);
                if (fontLoaded) {
                    window->draw(text);
                    if (drawCharacterName) window->draw(charText);
                }
            }
        }
        void Dialogue::Destroy()
        {
            novelSystem->SendMessage({"Destroy", this});
            if (TemporarySettings::dialogue == &text) TemporarySettings::dialogue = nullptr;
            if (TemporarySettings::name == &charText) TemporarySettings::name = nullptr;
        }
        void Dialogue::Resize(const unsigned int& width, const unsigned int& height)
        {
            charText.setCharacterSize((unsigned int)(characterSize * gs::scale));
            text.setCharacterSize((unsigned int)(characterSize * gs::scale));
            
            if (fontLoaded)
            {
                if (Skin::self && Skin::self->dialogue.outlineThickness != 0)
                    text.setOutlineThickness(Skin::self->dialogue.outlineThickness * gs::scale);
                if (drawCharacterName && charThickness)
                    charText.setOutlineThickness(charThickness * gs::scale);
                if (guiSystem)
                {
                    if (charString != L"") guiSystem->ResetResize();
                    guiSystem->Resize(width, height);
                }
                
                int textWidth = width, textXOffset = 0, textYOffset = 0;
                int nameTextWidth = width, nameTextXOffset = 0, nameTextYOffset = 0;
                if (Skin::self)
                {
                    GUIObject* textConstrains = nullptr;
                    if (gs::trueVerticalOrientation) { textConstrains = Skin::self->dialogue.textConstrainsV;
                        if (!textConstrains) textConstrains = Skin::self->dialogue.textConstrainsH; }
                    else { textConstrains = Skin::self->dialogue.textConstrainsH;
                        if (!textConstrains) textConstrains = Skin::self->dialogue.textConstrainsV; }
                    
                    GUIObject* dialogueRect = nullptr;
                    if (gs::trueVerticalOrientation) { dialogueRect = Skin::self->dialogue.dialogueRectV;
                        if (!dialogueRect) dialogueRect = Skin::self->dialogue.dialogueRectH; }
                    else { dialogueRect = Skin::self->dialogue.dialogueRectH;
                        if (!dialogueRect) dialogueRect = Skin::self->dialogue.dialogueRectV; }
                    
                    if (!textConstrains) textConstrains = dialogueRect;
                    
                    if (textConstrains)
                    {
                        textWidth = textConstrains->constrains.globalBounds.width;
                        textXOffset = textConstrains->constrains.globalBounds.left;
                        if (textString != L"" && afterRedLineShift < 0) textXOffset += afterRedLineShift*gs::scale;
                        textYOffset = textConstrains->constrains.globalBounds.top;
                    }
                    
                    if (dialogueRect)
                    {
                        workingArea.left = dialogueRect->constrains.globalBounds.left;
                        workingArea.width = dialogueRect->constrains.globalBounds.width;
                        workingArea.top = dialogueRect->constrains.globalBounds.top;
                        workingArea.height = dialogueRect->constrains.globalBounds.height;
                    }
                    else { workingArea.left = 0; workingArea.width = width; workingArea.top = 0; workingArea.height = height; }
                    
                    if (charString != L"")
                    {
                        GUIObject* nameConstrains = nullptr;
                        if (gs::trueVerticalOrientation) { nameConstrains = Skin::self->dialogue.nameConstrainsV;
                            if (!nameConstrains) nameConstrains = Skin::self->dialogue.nameConstrainsH; }
                        else { nameConstrains = Skin::self->dialogue.nameConstrainsH;
                            if (!nameConstrains) nameConstrains = Skin::self->dialogue.nameConstrainsV; }
                        if (!nameConstrains)
                        {
                            if (gs::trueVerticalOrientation) { nameConstrains = Skin::self->dialogue.nameRectV;
                                if (!nameConstrains) nameConstrains = Skin::self->dialogue.nameRectH; }
                            else { nameConstrains = Skin::self->dialogue.nameRectH;
                                if (!nameConstrains) nameConstrains = Skin::self->dialogue.nameRectV; }
                        }
                        if (nameConstrains)
                        {
                            nameTextWidth = nameConstrains->constrains.globalBounds.width;
                            nameTextXOffset = nameConstrains->constrains.globalBounds.left;
                            if (charString != L"" && afterRedLineShift < 0) nameTextXOffset += afterRedLineShift*gs::scale;
                            nameTextYOffset = nameConstrains->constrains.globalBounds.top;
                        }
                    }
                }
                
                if (charString != L"")
                    charText.setString(nss::GetStringWithLineBreaks(charText, charString, nameTextWidth, 0, afterRedLineShift*gs::scale));
                if (textString != L"")
                {
                    if (textAppearMode ==  textAppearModeEnum::printing)
                    {
                        currentString = L""; textAppearI = 0;
                        printingString = nss::GetStringWithLineBreaks(text, textString, textWidth, 0, afterRedLineShift*gs::scale);
                        for (size_t i = 0; i < textAppearPos; ++i)
                        {
                            while (printingString[textAppearI] == L'\n' || printingString[textAppearI] == L' ')
                                currentString += printingString[textAppearI++];
                            currentString += printingString[textAppearI++];
                        }
                        text.setString(currentString);
                    }
                    else text.setString(nss::GetStringWithLineBreaks(text, textString, textWidth, 0, afterRedLineShift*gs::scale));
                }
            
                text.setPosition(textXOffset, textYOffset);
                if (drawCharacterName) charText.setPosition(nameTextXOffset, nameTextYOffset);
            }
        }
        void Dialogue::SetCharacter(CharacterData* character)
        {
            if (character)
            {
                this->character = character;
                charThickness = (character->thicknessSet ? character->outlineThickness : (Skin::self ? Skin::self->dialogue.nameThickness : 0));
                charText.setOutlineThickness(charThickness);
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
            fontLoaded = text.getFont();
            
            charText.setCharacterSize(characterSize);
            if (Skin::self) { charText.setFillColor(Skin::self->dialogue.nameFillColor);
                charText.setOutlineColor(Skin::self->dialogue.nameOutlineColor);
                charThickness = Skin::self->dialogue.nameThickness; }
            else { charText.setFillColor(sf::Color::White); charText.setOutlineColor(sf::Color::Black); }
            
            charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
            charText.setOutlineColor(sf::Color(charText.getOutlineColor().r, charText.getOutlineColor().g, charText.getOutlineColor().b, alpha));
        }
        void Dialogue::SetDialogue(const sf::String& dialogue)
        {
            textString = dialogue;
            if (charString.length() != 0)
            {
                if (leftSpeechAddition != 0) textString.insert(textString.begin(), leftSpeechAddition);
                if (rightSpeechAddition != 0) textString += rightSpeechAddition;
            }
            
            printingString = textString;
            if (textAppearMode == textAppearModeEnum::printing)
            {
                currentString = L""; textAppearMax = base::GetLengthWONewLinesAndSpaces(textString);
                text.setString(currentString);
            }
            else text.setString(printingString);
            
            text.setFont(*ns::FontCollector::GetFont(fontName));
            fontLoaded = text.getFont();
            
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            if (Skin::self)
            {
                text.setFillColor(sf::Color(Skin::self->dialogue.fillColor.r, Skin::self->dialogue.fillColor.g, Skin::self->dialogue.fillColor.b, alpha));
                if (Skin::self->dialogue.outlineThickness != 0)
                    text.setOutlineColor(sf::Color(Skin::self->dialogue.outlineColor.r, Skin::self->dialogue.outlineColor.g, Skin::self->dialogue.outlineColor.b, alpha));
            }
            
            Resize(gs::width, gs::height);
        }
        void Dialogue::SetStateMode(modeEnum newMode)
        {
            gs::requestWindowRefresh = true;
            if (mode != newMode) { currentTime = 0.f; mode = newMode; }
        }
        void Dialogue::ReceiveMessage(MessageHolder &message) { if (message.info == "Show/Hide Interface") visible = !visible;
            else if (message.info == "FinishedExecute") guiSystem->ReceiveMessage(message); }
        void Dialogue::UpdateAlpha(bool mode)
        {
            if (guiSystem) { if (mode) guiSystem->SetAlphaIfBigger(alpha); else guiSystem->SetAlpha(alpha); }
            text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
            if (text.getOutlineThickness() != 0)
                text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, alpha));
            if (drawCharacterName)
            {
                charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                charText.setOutlineColor(sf::Color(charText.getOutlineColor().r, charText.getOutlineColor().g, charText.getOutlineColor().b, alpha));
            }
        }
        void Dialogue::Save(std::wofstream& wof)
        {
            if (!drawCharacterName) wof << L"text: " << textString << endl;
            if (drawCharacterName)
            {
                if (leftSpeechAddition != 0 || rightSpeechAddition != 0)
                {
                    std::wstring text = textString;
                    if (leftSpeechAddition != 0) text.erase(text.begin());
                    if (rightSpeechAddition != 0) text.erase(text.begin() + text.length() - 1);
                    wof << L"text: " << text << endl;
                } else wof << L"text: " << textString << endl;
                if (character) wof << L"cname: " << character->name << endl;
                else wof << L"cstr: " << charString << endl;
            }
            
            if (mode != waitingForInput)
            {
                wof << L"mode: " << mode << endl;
                if (mode == appearing || mode == disappearing) wof << L"currentTime: " << currentTime << endl;
                if (waitingTime != 2.f && mode == waitingForTime) wof << L"waitingTime: " << waitingTime << endl;
                if (mode == appearing && ((Skin::self && appearTime != Skin::self->dialogue.appearTime) || (!Skin::self && appearTime != 0.6f))) wof << L"appearTime: " << appearTime << endl;
            }
            if ((Skin::self && disappearTime != Skin::self->dialogue.disappearTime) || (!Skin::self && disappearTime != 0.6f)) wof << L"disappearTime: " << disappearTime << endl;
            if ((Skin::self && maxAlpha != Skin::self->dialogue.maxAlpha) || (!Skin::self && maxAlpha != 255)) wof << L"maxAlpha: " << maxAlpha << endl;
            if (textAppearMode != textAppearModeEnum::printing) wof << L"tmode: " << (int)textAppearMode << endl;
            if (textAppearMode == textAppearModeEnum::printing && textAppearPos != textAppearMax) wof << L"tpos: " << textAppearPos << endl;
            if (sendMessageBack != atDisappearing) wof << L"send: " << sendMessageBack << endl;
            if (!guiSystem) wof << L"ng" << endl;
        }
        std::pair<std::wstring, bool> Dialogue::Load(std::wifstream& wif)
        {
            mode = waitingForInput; bool setTextPosToMax{ true };
            std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"text: ")) textString = nss::ParseUntil(command, L'\n');
                else if (nss::Command(command, L"cname: "))
                {
                    std::wstring name = nss::ParseUntil(command, L'\n');
                    auto it = CharacterLibrary::find(name);
                    if (it != CharacterLibrary::library.end()) { SetCharacterName((*it).second->display); SetCharacter((*it).second); }
                }
                else if (nss::Command(command, L"cstr: ")) SetCharacterName(nss::ParseUntil(command, L'\n'));
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = modeEnum::appearing; break;
                        case 1: mode = modeEnum::waiting; break;
                        case 2: mode = modeEnum::waitingForTime; break;
                        case 4: mode = modeEnum::waitingForChoose; break;
                        case 5: mode = modeEnum::disappearing; break;
                        case 6: mode = modeEnum::deprecated; break;
                        default: mode = modeEnum::waitingForInput; break;
                    }
                }
                else if (nss::Command(command, L"currenttime: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"appeartime: ")) appearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"disappeartime: ")) disappearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"waitingtime: ")) waitingTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"tpos: ")) { textAppearPos = nss::ParseAsInt(command); setTextPosToMax = false; }
                else if (nss::Command(command, L"tmode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: textAppearMode = textAppearModeEnum::fading; setTextPosToMax = false; break;
                        default: textAppearMode = textAppearModeEnum::printing; break;
                    }
                }
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
                else if (nss::Command(command, L"ng")) noguiSystem = true;
                if (wif.eof()) done = true;
            }
            
            if (mode == modeEnum::appearing) alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
            else if (mode == modeEnum::disappearing) alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
            else alpha = maxAlpha; UpdateAlpha();
            bool onHold{ !((sendMessageBack == sendMessageBackEnum::noMessage) || (sendMessageBack == sendMessageBackEnum::atAppearance && mode > 0) || (sendMessageBack == sendMessageBackEnum::atDisappearing && mode > 4)) };
            
            if (setTextPosToMax) textAppearPos = base::GetLengthWONewLinesAndSpaces(textString);
            
            return { line, onHold };
        }
        
        
        
        
        
        
        
        Choose::Choose(GUISystem* guiSystem) : guiSystem(guiSystem), Savable(L"Choose") { }
        void Choose::Init()
        {
            if (Skin::self)
            {
                appearTime = Skin::self->choose.appearTime;
                disappearTime = Skin::self->choose.disappearTime;
                maxAlpha = Skin::self->choose.maxAlpha;
                characterSize = Skin::self->choose.characterSize;
                fontName = Skin::self->choose.fontName;
            }
            if (guiSystem) guiSystem->Resize(gs::width, gs::height);
        }
        void Choose::Update(const sf::Time& elapsedTime)
        {
            if (guiSystem) guiSystem->Update(elapsedTime);
            switch (mode)
            {
                case appearing: gs::requestWindowRefresh = true;
                    if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime) {
                        alpha = maxAlpha; currentTime = 0.f; mode = waitingForInput;
                        if (sendMessageBack == atAppearance) novelSystem->SendMessage({"UnHold", this});
                    } else alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
                    button.setAlpha(alpha);
                    break;
                    
                case disappearing: gs::requestWindowRefresh = true;
                    if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime) {
                        alpha = 0; currentTime = 0.f; mode = deprecated;
                        if (sendMessageBack == atDeprecated) novelSystem->SendMessage({"UnHold", this});
                    } else alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    button.setAlpha(alpha);
                    break;

                case deprecated: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                default: break;
            }
        }
        void Choose::PollEvent(sf::Event& event)
        {
            if (mode == waitingForInput)
            {
                if (guiSystem && visible) guiSystem->PollEvent(event);
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) visible = !visible;
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
                            button.setPosition(gs::width/2, yy);
                            
                            if ((found = button.PollEvent(event)))
                            {
                                int until = i+1 < choices.size() ? choiceStart[i+1] : actions.size();
                                for (int j = until - 1; j >= choiceStart[i]; --j)
                                    novelSystem->SendMessage({"Push", actions[j]});
                                mode = disappearing;
                                if (sendMessageBack == atDisappearing) novelSystem->SendMessage({"ChooseUnHold", this});
                            }
                            
                            yy += button.text.getGlobalBounds().height + 10*gs::scaley;
                        }
                    }
                }
            }
        }
        void Choose::Draw(sf::RenderWindow* window)
        {
            if (!visible) return;
            
            if (guiSystem) { guiSystem->SetAlpha(alpha); guiSystem->Draw(window); }
            int yy = startingYY;
            for (int i = 0; i < choices.size(); ++i)
            {
                button.setString(choices[i]);
                button.setPosition(gs::width/2, yy);
                button.draw(window);
                
                yy += button.text.getGlobalBounds().height + 10*gs::scaley;
            }
        }
        void Choose::Destroy() { novelSystem->SendMessage({"Destroy", this}); }
        void Choose::Resize(const unsigned int& width, const unsigned int& height)
        {
            if (guiSystem) guiSystem->Resize(width, height);
            button.Resize(width, height);
            if (choices.size()) button.setString(choices[0]);
            startingYY = (gs::height/2 - (float)((button.text.getGlobalBounds().height + 10) * choices.size())/2) + button.text.getGlobalBounds().height/2;
        }
        void Choose::SetStateMode(modeEnum newMode) { if (mode != newMode) { currentTime = 0.f; mode = newMode; } }
        void Choose::AddChoice(const std::wstring& line) { choices.push_back(line); choiceStart.push_back(actions.size()); }
        void Choose::AddAction(const std::wstring& line) { actions.push_back(line); }
        void Choose::InitChoose()
        {
            /*cout << "   choices: " << endl;
            for (int i = 0; i < choices.size; ++i)
                cout << utf8(choices[i]) << endl;
            
            cout << "   choicesStart: " << endl;
            for (int i = 0; i < choiceStart.size; ++i)
                cout << choiceStart[i] << endl;
            
            cout << "   actions: " << endl;
            for (int i = 0; i < actions.size; ++i)
                cout << utf8(actions[i]) << endl;*/
            
            if (choices.size() == 0)
            {
                if (sendMessageBack != noMessage) novelSystem->SendMessage({"UnHold", this});
                novelSystem->PopComponent(this);
            }
            else
            {
                button.setFont(fontName);
                button.characterSize = characterSize;
                button.ignoreWasPressed = true;
                button.halign = Halign::Center;
                button.valign = Valign::Center;
                
                Resize(gs::width, gs::height);
            }
        }
        void Choose::ReceiveMessage(MessageHolder &message) { if (message.info == "Show/Hide Interface") visible = !visible;
            else if (message.info == "FinishedExecute") guiSystem->ReceiveMessage(message); }
        void Choose::Save(std::wofstream& wof)
        {
            wof << "b1" << endl;
            for (auto& s : actions) wof << s << endl;
            wof << "e" << endl << "b2" << endl;
            for (auto& s : choices) wof << s << endl;
            wof << "e" << endl << "b3" << endl;
            for (auto& s : choiceStart) wof << s << endl;
            wof << "e" << endl;
            
            if (mode != waitingForInput)
            {
                wof << L"mode: " << mode << endl;
                if (mode == appearing || mode == disappearing) wof << L"currentTime: " << currentTime << endl;
                if (mode == appearing && ((Skin::self && appearTime != Skin::self->choose.appearTime) || (!Skin::self && appearTime != 0.6f))) wof << L"appearTime: " << appearTime << endl;
            }
            if ((Skin::self && disappearTime != Skin::self->choose.disappearTime) || (!Skin::self && disappearTime != 0.6f)) wof << L"disappearTime: " << disappearTime << endl;
            if ((Skin::self && maxAlpha != Skin::self->dialogue.maxAlpha) || (!Skin::self && maxAlpha != 255)) wof << L"maxAlpha: " << maxAlpha << endl;
            if (sendMessageBack != atDisappearing) wof << L"send: " << sendMessageBack << endl;
            if (!guiSystem) wof << L"ng" << endl;
        }
        std::pair<std::wstring, bool> Choose::Load(std::wifstream& wif)
        {
            mode = waitingForInput; bool setTextPosToMax{ true };
            std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"b1")) {
                    std::getline(wif, line); while (line != L"e" && !wif.eof()) { actions.push_back(line); std::getline(wif, line); } }
                else if (nss::Command(command, L"b2")) {
                    std::getline(wif, line); while (line != L"e" && !wif.eof()) { choices.push_back(line); std::getline(wif, line); } }
                else if (nss::Command(command, L"b3")) {
                    std::getline(wif, line); while (line != L"e" && !wif.eof()) { choiceStart.push_back(base::atoi(line)); std::getline(wif, line); } }
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = modeEnum::appearing; break;
                        case 2: mode = modeEnum::disappearing; break;
                        case 3: mode = modeEnum::deprecated; break;
                        default: mode = modeEnum::waitingForInput; break;
                    }
                }
                else if (nss::Command(command, L"currenttime: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"appeartime: ")) appearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"disappeartime: ")) disappearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"visible: ")) visible = nss::ParseAsBool(command);
                else if (nss::Command(command, L"maxalpha: ")) maxAlpha = nss::ParseAsInt(command);
                else if (nss::Command(command, L"send: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: sendMessageBack = sendMessageBackEnum::noMessage; break;
                        case 1: sendMessageBack = sendMessageBackEnum::atAppearance; break;
                        case 3: sendMessageBack = sendMessageBackEnum::atDeprecated; break;
                        default: sendMessageBack = sendMessageBackEnum::atDisappearing; break;
                    }
                }
                //else if (nss::Command(command, L"ng")) noguiSystem = true;
                if (wif.eof()) done = true;
            }
            
            if (mode == modeEnum::appearing) alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
            else if (mode == modeEnum::disappearing) alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
            else alpha = maxAlpha; button.setAlpha(alpha);
            bool onHold{ !((sendMessageBack == sendMessageBackEnum::noMessage) || (sendMessageBack == sendMessageBackEnum::atAppearance && mode > 0) || (sendMessageBack == sendMessageBackEnum::atDisappearing && mode > 1)) };
            
            cout << "choices.size(): " << choices.size() << endl;
            
            return { line, onHold };
        }
    }
}
