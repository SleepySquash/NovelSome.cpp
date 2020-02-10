//
//  SettingsUI.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 30/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "SettingsUI.hpp"

namespace ns
{
    void SettingsUI::Init()
    {
        backButton.setString(L"< " + lang::get("BACK"));
        backButton.setFont(L"Pacifica.ttf");
        backButton.setCharacterSize(81);
        backButton.halign = Halign::Left;
        backButton.valign = Valign::Bottom;
        backButton.characterScale = true;
        blackScreenShape.setFillColor(sf::Color(0,0,0,170));
        
        customizationButton.setFont(L"Pacifica.ttf");
        customizationButton.setCharacterSize(50);
        customizationButton.setString(lang::get("Customization"));
        graphicsButton.setFont(L"Pacifica.ttf");
        graphicsButton.setCharacterSize(50);
        graphicsButton.setString(lang::get("Graphics"));
        soundButton.setFont(L"Pacifica.ttf");
        soundButton.setCharacterSize(50);
        soundButton.setString(lang::get("Sound"));
        networkButton.setFont(L"Pacifica.ttf");
        networkButton.setCharacterSize(50);
        networkButton.setString(lang::get("Network"));
        
        tempButton.setString(L"Привет! Я кнопка справа.");
        tempButton.setCharacterSize(60); tempButton.thickness = 2;
        tempButton.setFont(L"Pacifica.ttf"); tempButton.halign = Halign::Right;
    }
    void SettingsUI::Destroy() { if (active) CleanUp(); }
    void SettingsUI::Update(const sf::Time& elapsedTime)
    {
        if (!active) return;
        switch (mode)
        {
            case appearing:      //currentTime = appearTime;
                if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                if (currentTime >= appearTime) { alpha = 255; currentTime = 0.f; mode = existing; }
                else alpha = (sf::Uint8)(255 * (currentTime / appearTime));
                UpdateAlpha(); gs::requestWindowRefresh = true;
                break;
                
            case disappearing:     //currentTime = disappearTime;
                if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                if (currentTime >= disappearTime) { active = false; CleanUp(); alpha = 0; currentTime = 0.f; mode = existing; }
                else alpha = (sf::Uint8)(255 - (255 * (currentTime / disappearTime)));
                UpdateAlpha(); gs::requestWindowRefresh = true;
                break;
            default: break;
        }
    }
    void SettingsUI::CleanUp() { /*ic::DeleteImage(L"Data/Neko/Person/Vanilla.png");*/ }
    void SettingsUI::PollEvent(sf::Event& event)
    {
        if (!active || !gs::isActiveInterface(this)) return;
        
        if (backButton.PollEvent(event) || (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) entity->SendMessage({"SettingsUI :: Close"});
        else if (customizationButton.PollEvent(event)) page = Page::customization;
        else if (graphicsButton.PollEvent(event)) page = Page::graphics;
        else if (soundButton.PollEvent(event)) page = Page::sound;
        else if (networkButton.PollEvent(event)) page = Page::network;
        else if (tempButton.PollEvent(event)) { gs::drawGUIBoundaries = !gs::drawGUIBoundaries;
            if (gs::drawGUIBoundaries) tempButton.setString(L"Да, рисуем границы!!!");
            else tempButton.setString(L"О нет! Границы не рисуем!"); }
    }
    void SettingsUI::Resize(const unsigned int& width, const unsigned int& height)
    {
        if (!active) return;
        
        blackScreenShape.setSize({(float)gs::width, (float)gs::height});
        backButton.Resize(width, height);
        backButton.setPosition(40*gs::scalex, gs::height - 40*gs::scaley);
        
        tempButton.Resize(width, height);
        tempButton.setPosition(width/3, 3*height/4);
        
        customizationButton.Resize(width, height);
        graphicsButton.Resize(width, height);
        soundButton.Resize(width, height);
        networkButton.Resize(width, height);
        if (gs::verticalOrientation)
        {
            float yy = 5*gs::scale;
            customizationButton.setPosition(gs::width/2, yy); yy += customizationButton.text.getGlobalBounds().height;
            graphicsButton.setPosition(gs::width/2, yy); yy += graphicsButton.text.getGlobalBounds().height;
            soundButton.setPosition(gs::width/2, yy); yy += soundButton.text.getGlobalBounds().height;
            networkButton.setPosition(gs::width/2, yy);
        }
        else
        {
            //float xx = customizationButton.text.getGlobalBounds().width + graphicsButton.text.getGlobalBounds().width + soundButton.text.getGlobalBounds().width + networkButton.text.getGlobalBounds().width + 20*gs::scale;
            float xx = customizationButton.text.getGlobalBounds().width/2 + networkButton.text.getGlobalBounds().width/2;
            customizationButton.setPosition((gs::width - xx)/4, 5*gs::scaley);
            graphicsButton.setPosition(2*(gs::width - xx)/4, 5*gs::scaley);
            soundButton.setPosition(3*(gs::width - xx)/4, 5*gs::scaley);
            networkButton.setPosition(4*(gs::width - xx)/4, 5*gs::scaley);
        }
    }
    void SettingsUI::Draw(sf::RenderWindow* window)
    {
        if (!active || gs::ignoreDraw) return;
        
        window->draw(blackScreenShape);
        backButton.draw(window);
        customizationButton.draw(window);
        graphicsButton.draw(window);
        soundButton.draw(window);
        networkButton.draw(window);
        
        tempButton.draw(window);
    }
    void SettingsUI::ReceiveMessage(MessageHolder& message)
    {
        if (message.info == "SettingsUI") Switch(!active);
        else if (message.info == "SettingsUI :: Close") Switch(false);
    }
    void SettingsUI::Switch(const bool& on)
    {
        if (on && !active) { gs::PushInterface(this); active = true; mode = appearing; entity->SortAbove(this); Resize(gs::width, gs::height); }
        else if (!on && active) { gs::RemoveInterface(this); mode = disappearing; }
    }
    void SettingsUI::UpdateAlpha()
    {
        blackScreenShape.setFillColor(sf::Color(blackScreenShape.getFillColor().r, blackScreenShape.getFillColor().g, blackScreenShape.getFillColor().b, 170.f * ((float)alpha/255)));
        blackScreenShape.setOutlineColor(sf::Color(blackScreenShape.getOutlineColor().r, blackScreenShape.getOutlineColor().g, blackScreenShape.getOutlineColor().b, 170.f * ((float)alpha/255.f)));
        backButton.setAlpha(alpha); customizationButton.setAlpha(alpha);
        graphicsButton.setAlpha(alpha); soundButton.setAlpha(alpha); networkButton.setAlpha(alpha);
    }
}
