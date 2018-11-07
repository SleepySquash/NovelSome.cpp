//
//  GUIInterface.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/10/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "GUIInterface.hpp"

namespace ns
{
    namespace GUI
    {
        void TextButton::SetPosition(const int& X, const int& Y)
        {
            x = X; y = Y;
            text.setPosition(x * ((float)gs::width / gs::relativeWidth), y * ((float)gs::height / gs::relativeHeight));
        }
        void TextButton::SetString(const std::wstring& line)
        {
            textString = line;
            text.setString(line);
            
            if (fontLoaded)
            {
                if (halign == halignEnum::left)
                    text.setOrigin(0, text.getOrigin().y);
                else if (halign == halignEnum::center)
                    text.setOrigin(text.getLocalBounds().width/2, text.getOrigin().y);
                else if (halign == halignEnum::right)
                    text.setOrigin(text.getLocalBounds().width, text.getOrigin().y);
                
                if (valign == valignEnum::top)
                    text.setOrigin(text.getOrigin().x, 0);
                else if (valign == valignEnum::middle)
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height/2);
                else if (valign == valignEnum::bottom)
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height);
            }
        }
        void TextButton::SetFont(const std::wstring& font)
        {
            fontName = font;
            if ((fontLoaded = (fc::GetFont(fontName) != nullptr)))
                text.setFont(*fc::GetFont(fontName));
        }
        void TextButton::Draw(sf::RenderWindow* window)
        {
            if (fontLoaded)
                window->draw(text);
        }
        void TextButton::SetAlpha(const sf::Int8& alpha)
        {
            if (fontLoaded)
            {
                unsigned char realAlpha = sf::Int8((unsigned char)alpha * ((float)maxAlpha/255));
                text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, realAlpha));
                text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, realAlpha));
            }
        }
        void TextButton::Resize(unsigned int width, unsigned int height)
        {
            if (fontLoaded)
            {
                text.setCharacterSize((unsigned int)(characterSize * gs::scale));
                text.setPosition(x * ((float)width / gs::relativeWidth), y * ((float)height / gs::relativeHeight));
                
                if (halign == halignEnum::left)
                    text.setOrigin(0, text.getOrigin().y);
                else if (halign == halignEnum::center)
                    text.setOrigin(text.getLocalBounds().width/2, text.getOrigin().y);
                else if (halign == halignEnum::right)
                    text.setOrigin(text.getLocalBounds().width, text.getOrigin().y);
                
                if (valign == valignEnum::top)
                    text.setOrigin(text.getOrigin().x, 0);
                else if (valign == valignEnum::middle)
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height/2);
                else if (valign == valignEnum::bottom)
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height);
            }
        }
        void TextButton::PollEvent(sf::Event& event)
        {
            if (fontLoaded)
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    released = false;
                }
                else if (event.type == sf::Event::TouchBegan)
                {
                    released = false;
                }
                else if (event.type == sf::Event::TouchEnded)
                {
                    int left = text.getPosition().x - text.getOrigin().x;
                    int top = text.getPosition().y - text.getOrigin().y;
                    pressed = (event.touch.x >= left && event.touch.x <= left + text.getLocalBounds().width &&
                               event.touch.y >= top && event.touch.y <= top + text.getLocalBounds().height);
                    
                    if (pressed)
                        event = sf::Event();
                    
                    released = pressed;
                    pressed = false;
                }
                else if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left))
                {
                    int left = text.getPosition().x - text.getOrigin().x;
                    int top = text.getPosition().y + text.getLocalBounds().height/2.5 - text.getOrigin().y;
                    
                    pressed = (event.mouseButton.x >= left && event.mouseButton.x <= (left + text.getLocalBounds().width) &&
                               event.mouseButton.y >= top && event.mouseButton.y <= (top + text.getLocalBounds().height));
                    
                    if (pressed)
                        event = sf::Event();
                    
                    released = pressed;
                    pressed = false;
                }
            }
        }
        bool TextButton::Released()
        {
            if (released)
            {
                released = false;
                return true;
            }
            return false;
        }
    }
}
