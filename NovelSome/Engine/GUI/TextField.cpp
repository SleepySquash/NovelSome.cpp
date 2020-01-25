//
//  TextField.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 24.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#include "TextField.hpp"

namespace ns
{
    namespace GUI
    {
        void TextField::Init()
        {
            shape.setFillColor(sf::Color(0, 0, 0, 130));
            shape.setOutlineColor(sf::Color(255,255,255, 100));
        }
        bool TextField::PollEvent(sf::Event& event)
        {
            if (active && event.type == sf::Event::TextEntered)
            {
                elapsedBlink = 0.5f; drawBlink = true;
                if (event.text.unicode == 8) { if (string.getSize() > 0) string.erase(string.getSize() - 1); }
                else if (event.text.unicode == 13) active = false;
                else if (event.text.unicode != 9) string += event.text.unicode; text.setString(string);
            }
            else if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded || (active && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Enter))
            {
                if (event.type == sf::Event::KeyPressed) active = false;
                else if (event.type == sf::Event::MouseButtonReleased) active = shape.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y);
                else active = shape.getGlobalBounds().contains(event.touch.x, event.touch.y);
            }
            
            return setActive(active);
        }
        bool TextField::setActive(const bool& act)
        {
            if (wasActive != active)
            {
                // TODO: При включённой клавиатуре, если окошко не попадает в экран, то вытягивать его наверх.
                wasActive = active;
                if (active)
                {
                    sf::Keyboard::setVirtualKeyboardVisible(true);
                    shape.setOutlineThickness(2 * gs::scale);
                    shape.setOutlineColor(sf::Color(255,255,255, 255));
                    shape.setFillColor(sf::Color(0, 0, 0, 210));
                    elapsedBlink = 0.5f; drawBlink = true;
                }
                else
                {
                    sf::Keyboard::setVirtualKeyboardVisible(false);
                    shape.setOutlineThickness(gs::scale);
                    shape.setOutlineColor(sf::Color(255,255,255, 100));
                    shape.setFillColor(sf::Color(0, 0, 0, 130));
                    return text.getString().getSize();
                }
            }
            return false;
        }
        void TextField::Resize(const unsigned int& width, const unsigned int& height)
        {
            //if (drawWhenEmpty) if (text.getString().getSize() == 0) text.setString(textWhenEmpty);
            text.setCharacterSize(characterSize * gs::scScale);
            shape.setOutlineThickness(gs::scale);
        }
        void TextField::Draw(sf::RenderWindow* window)
        {
            window->draw(shape);
            window->draw(text);
        }
        void TextField::setPosition(float x, float y)
        {
            shape.setPosition(x, y);
            if (fontLoaded)
            {
                sf::String before = text.getString();
                text.setString("1ASD");
                text.setOrigin(0, text.getGlobalBounds().height/2);
                text.setString(before);
                text.setPosition(x + 5*gs::scale, y + shape.getSize().y/2 - 5*gs::scaley);
            }
        }
        void TextField::setFont(const std::wstring& fontname)
        {
            sf::Font* font = fc::GetFont(fontname);
            if ((fontLoaded = font)) text.setFont(*font);
        }
    }
}
