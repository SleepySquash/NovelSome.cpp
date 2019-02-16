//
//  Misc.cpp
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
        GamePause::GamePause()
        {
            shape.setFillColor(sf::Color(0,0,0,0));
            menuBackButton.setCharacterSize(60);
            menuBackButton.setFont(L"Pacifica.ttf");
            menuBackButton.setString(L"Back to menu");
        }
        GamePause::~GamePause() { gs::isPause = false; }
        void GamePause::Update(const sf::Time& elapsedTime)
        {
            if (alpha != 0 && novel != nullptr)
                novel->skin.gamePauseGUI.Update(elapsedTime);
            
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
                    if (alpha == 0 && novel) Resize(gs::width, gs::height);
                    
                    if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime)
                    {
                        alpha = maxAlpha;
                        currentTime = 0.f;
                        mode = appeared;
                    }
                    else alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    if (novel) novel->skin.gamePauseGUI.SetAlpha(alpha);
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
                    if (novel) novel->skin.gamePauseGUI.SetAlpha(alpha);
                    menuBackButton.setAlpha(alpha);
                    break;
                    
                default: break;
            }
        }
        void GamePause::PollEvent(sf::Event& event)
        {
            if (gs::isPause && menuBackButton.PollEvent(event)) { gs::isPause = false; if (novel) entity->PopComponent(novel); }
            if (novel) novel->skin.gamePauseGUI.PollEvent(event);
            if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                || (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right))
            {
                gs::isPause = !gs::isPause;
                mode = gs::isPause? appearing : disappearing;
                currentTime = 0.f;
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
                        
                        ns::GlobalSettings::isPause = !ns::GlobalSettings::isPause;
                        mode = ns::GlobalSettings::isPause? appearing : disappearing;
                        currentTime = 0.f;
                    }
                }
            }
            else if (event.type == sf::Event::LostFocus) { if (!gs::isPause) {gs::isPause = true; mode = appearing; currentTime = 0.f; }}
        }
        void GamePause::Draw(sf::RenderWindow *window)
        {
            if (mode != waiting && novel)
            {
                novel->skin.gamePauseGUI.Draw(window);
                menuBackButton.Draw(window);
            }
        }
        void GamePause::Resize(unsigned int width, unsigned int height)
        {
            if (mode != waiting && novel)
            {
                novel->skin.gamePauseGUI.Resize(width, height);
                
                menuBackButton.Resize(width, height);
                menuBackButton.setPosition(width/2, height - height/5);
            }
        }
        
        
        
        
        
        
        void Waiting::Update(const sf::Time& elapsedTime)
        {
            if (novel != nullptr)
            {
                if (currentTime < waitingTime)
                    currentTime += elapsedTime.asSeconds();
                else
                {
                    novel->UnHold(this);
                    GetNovelSystem()->PopComponent(this);
                }
            }
        }
        void Waiting::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
    }
}
