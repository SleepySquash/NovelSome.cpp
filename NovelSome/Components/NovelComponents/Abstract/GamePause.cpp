//
//  GamePause.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 10/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "GamePause.hpp"

namespace ns
{
    namespace NovelComponents
    {
        GamePause::GamePause(GUISystem* guiSystem) : guiSystem(guiSystem) { shape.setFillColor(sf::Color(0,0,0,0)); }
        GamePause::~GamePause() { gs::isPause = false; }
        void GamePause::Update(const sf::Time& elapsedTime)
        {
            if (alpha && guiSystem) guiSystem->Update(elapsedTime);
            
            if (countdownLastTouchedMoment)
            {
                if (lastTouchedMoment > 0.f)
                    lastTouchedMoment -= elapsedTime.asSeconds();
                if (lastTouchedMoment <= 0.f)
                {
                    lastTouchedMoment = 0.f;
                    countdownLastTouchedMoment = false;
                }
            }
            switch (mode)
            {
                case appearing: gs::requestWindowRefresh = true;
                    if (alpha == 0) Resize(gs::width, gs::height);
                    
                    if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime)
                    {
                        alpha = maxAlpha;
                        currentTime = 0.f;
                        mode = appeared;
                    }
                    else alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    if (guiSystem) guiSystem->SetAlpha(alpha);
                    menuBackButton.setAlpha(alpha);
                    break;
                    
                case disappearing: gs::requestWindowRefresh = true;
                    if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0;
                        currentTime = 0.f;
                        mode = waiting;
                    }
                    else alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    if (guiSystem) guiSystem->SetAlpha(alpha);
                    menuBackButton.setAlpha(alpha);
                    break;
                    
                default: break;
            }
        }
        void GamePause::PollEvent(sf::Event& event)
        {
            //if (gs::isPause && menuBackButton.PollEvent(event)) { gs::isPause = false; entity->SendMessage({"GamePause :: Return to menu"}); }
            if (gs::ignoreEvent) return;
            if (guiSystem) guiSystem->PollEvent(event);
            if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                || (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right))
            {
                gs::isPause = !gs::isPause;
                entity->SendMessage({"GamePause"});
            }
            else if (event.type == sf::Event::TouchBegan)
            {
                if (event.touch.finger == 0)
                {
                    countdownLastTouchedMoment = true;
                    lastTouchedMoment = 0.03f;
                }
                else if (event.touch.finger == 1)
                {
                    if (countdownLastTouchedMoment && lastTouchedMoment > 0)
                    {
                        lastTouchedMoment = 0.f;
                        countdownLastTouchedMoment = false;
                        gs::isPause = !gs::isPause;
                        entity->SendMessage({"GamePause"});
                    }
                }
            }
            else if (event.type == sf::Event::LostFocus) { if (!gs::isPause) {gs::isPause = true; mode = appearing; currentTime = 0.f; }}
        }
        void GamePause::Draw(sf::RenderWindow *window) { if (mode != waiting && guiSystem && !gs::ignoreDraw) guiSystem->Draw(window); }
        void GamePause::Resize(const unsigned int& width, const unsigned int& height) { if (mode != waiting && guiSystem) guiSystem->Resize(width, height); }
        void GamePause::ReceiveMessage(MessageHolder &message)
        {
            if (message.info == "FinishedExecute") guiSystem->ReceiveMessage(message);
            else if (message.info == "GamePause") { mode = gs::isPause? appearing : disappearing; currentTime = 0.f; }
        }
    }
}
