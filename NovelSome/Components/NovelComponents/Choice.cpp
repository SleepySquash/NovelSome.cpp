//
//  Choice.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 13.02.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#include "Choice.hpp"

namespace ns
{
    namespace NovelComponents
    {
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
                case Mode::Appear: gs::requestWindowRefresh = true;
                    if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime) {
                        alpha = maxAlpha; currentTime = 0.f; mode = Mode::WaitingForInput;
                        if (messageBack == MessageBack::AtAppearance) novelSystem->SendMessage({"UnHold", this});
                    } else alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
                    button.setAlpha(alpha);
                    break;
                    
                case Mode::Disapper: gs::requestWindowRefresh = true;
                    if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime) {
                        alpha = 0; currentTime = 0.f; mode = Mode::Deprecate;
                        if (messageBack == MessageBack::AtDeprecated) novelSystem->SendMessage({"UnHold", this});
                    } else alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    button.setAlpha(alpha);
                    break;

                case Mode::Deprecate: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                default: break;
            }
        }
        void Choose::PollEvent(sf::Event& event)
        {
            if (mode == Mode::WaitingForInput)
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
                                mode = Mode::Disapper;
                                if (messageBack == MessageBack::AtDisappearance) novelSystem->SendMessage({"ChooseUnHold", this});
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
        void Choose::SetStateMode(const Mode& newMode) { if (mode != newMode) { currentTime = 0.f; mode = newMode; } }
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
                if (messageBack != MessageBack::No) novelSystem->SendMessage({"UnHold", this});
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
            
            if (mode != Mode::WaitingForInput)
            {
                wof << L"mode: " << (int)mode << endl;
                if (mode == Mode::Appear || mode == Mode::Disapper) wof << L"currentTime: " << currentTime << endl;
                if (mode == Mode::Appear && ((Skin::self && appearTime != Skin::self->choose.appearTime) || (!Skin::self && appearTime != 0.6f))) wof << L"appearTime: " << appearTime << endl;
            }
            if ((Skin::self && disappearTime != Skin::self->choose.disappearTime) || (!Skin::self && disappearTime != 0.6f)) wof << L"disappearTime: " << disappearTime << endl;
            if ((Skin::self && maxAlpha != Skin::self->dialogue.maxAlpha) || (!Skin::self && maxAlpha != 255)) wof << L"maxAlpha: " << maxAlpha << endl;
            if (messageBack != MessageBack::AtDisappearance) wof << L"send: " << (int)messageBack << endl;
            if (!guiSystem) wof << L"ng" << endl;
        }
        std::pair<std::wstring, bool> Choose::Load(std::wifstream& wif)
        {
            mode = Mode::WaitingForInput; bool setTextPosToMax{ true };
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
                        case 0: mode = Mode::Appear; break;
                        case 1: mode = Mode::Disapper; break;
                        case 3: mode = Mode::Deprecate; break;
                        default: mode = Mode::WaitingForInput; break;
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
                        case 0: messageBack = MessageBack::No; break;
                        case 1: messageBack = MessageBack::AtAppearance; break;
                        case 3: messageBack = MessageBack::AtDeprecated; break;
                        default: messageBack = MessageBack::AtDisappearance; break;
                    }
                }
                //else if (nss::Command(command, L"ng")) noguiSystem = true;
                if (wif.eof()) done = true;
            }
            
            if (mode == Mode::Appear) alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
            else if (mode == Mode::Disapper) alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
            else alpha = maxAlpha; button.setAlpha(alpha);
            /// bool onHold{ !((sendMessageBack == sendMessageBackEnum::noMessage) || (sendMessageBack == sendMessageBackEnum::atAppearance && mode > 0) || (sendMessageBack == sendMessageBackEnum::atDisappearing && mode > 1)) };
            // bool onHold{ ((messageBack == MessageBack::AtAppearance && mode == Mode::Appear) || (messageBack == MessageBack::AtDeprecated && mode == Mode::Disapper) || (messageBack != MessageBack::No && (mode == Mode::WaitingForInput || mode == Mode::Appear))) };
            bool onHold{ !((messageBack == MessageBack::No) || (messageBack == MessageBack::AtAppearance && (mode == Mode::Exist || mode == Mode::Disapper || mode == Mode::Deprecate || mode == Mode::WaitingForInput)) || (messageBack == MessageBack::AtDisappearance && (mode == Mode::Disapper || mode == Mode::Deprecate))) };
            
            cout << "choices.size(): " << choices.size() << endl;
            
            return { line, onHold };
        }
    }
}
