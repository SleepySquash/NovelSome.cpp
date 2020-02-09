//
//  RoundedRectangleButton.cpp
//  NekoUI
//
//  Created by Никита Исаенко on 29.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#include "RoundedRectangleButton.hpp"

namespace ns
{
    namespace GUI
    {
        RoundedRectangleButton::RoundedRectangleButton()
        {
            text.setFillColor(normalColor);
            shape.setFillColor(snormalColor);
            if (drawOutline) text.setOutlineColor(onormalColor);
            if (drawOutlineShape) shape.setOutlineColor(sonormalColor);
            shape.setCornerPointCount(5);
        }
        void RoundedRectangleButton::Draw(sf::RenderTarget* window)
        {
            if (loaded && visible)
            {
                if (shape.getSize().x < text.getGlobalBounds().width) { text.setScale(shape.getSize().x/(text.getGlobalBounds().width), 1); }
                if (index == pressedIndex)
                {
                    if (anyButtonPressed) {
                        if (drawOutlineShape) shape.setOutlineColor({sopressColor.r, sopressColor.g, sopressColor.b, ralpha});
                        if (drawOutline) text.setOutlineColor({opressColor.r, opressColor.g, opressColor.b, alpha});
                        shape.setFillColor({spressColor.r, spressColor.g, spressColor.b, ralpha});
                        text.setFillColor({pressColor.r, pressColor.g, pressColor.b, alpha}); }
                    else if (wasHovered) {
                        text.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, alpha));
                        shape.setFillColor(sf::Color(shoverColor.r, shoverColor.g, shoverColor.b, ralpha));
                        if (drawOutline) text.setOutlineColor(sf::Color(ohoverColor.r, ohoverColor.g, ohoverColor.b, ralpha));
                        if (drawOutlineShape) shape.setFillColor(sf::Color(shoverColor.r, shoverColor.g, shoverColor.b, ralpha)); }
                }
                if (drawShape) window->draw(shape);
                window->draw(text);
                if ((anyButtonPressed || wasHovered) && index == pressedIndex) {
                    if (drawOutlineShape) shape.setOutlineColor({sonormalColor.r, sonormalColor.g, sonormalColor.b, ralpha});
                    if (drawOutline) text.setOutlineColor({onormalColor.r, onormalColor.g, onormalColor.b, alpha});
                    shape.setFillColor({snormalColor.r, snormalColor.g, snormalColor.b, ralpha});
                    text.setFillColor({normalColor.r, normalColor.g, normalColor.b, alpha}); }
                text.setScale(1, 1);
            }
        }
        void RoundedRectangleButton::Resize(unsigned int width, unsigned int height)
        {
            shape.setCornersRadius(radius*gs::scale);
            if (characterScale) {
                text.setCharacterSize(characterSize * gs::scScale);
                if (drawOutline) text.setOutlineThickness(thickness * gs::scScale);
                if (drawOutlineShape) shape.setOutlineThickness(thickness * gs::scScale);
            } else {
                text.setCharacterSize(characterSize * gs::scale);
                if (drawOutline) text.setOutlineThickness(thickness * gs::scale);
                if (drawOutlineShape) shape.setOutlineThickness(thickness * gs::scale); }
            text.setScale(1, 1);
        }
        bool RoundedRectangleButton::PollEvent(sf::Event& event)
        {
            if (!active) return false;
            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchEnded)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                if (pressedIndex == index && (anyButtonPressed || ignoreWasPressed))
                {
                    if (drawOutlineShape) shape.setOutlineColor({sonormalColor.r, sonormalColor.g, sonormalColor.b, ralpha});
                    if (drawOutline) text.setOutlineColor({onormalColor.r, onormalColor.g, onormalColor.b, alpha});
                    text.setFillColor({normalColor.r, normalColor.g, normalColor.b, alpha});
                    shape.setFillColor({snormalColor.r, snormalColor.g, snormalColor.b, ralpha});
                    bool constains = shape.getGlobalBounds().contains(dot.x, dot.y);
                    if (constains) event = sf::Event();
                    anyButtonPressed = false;
                    
                    return constains;
                }
            }
            else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchBegan)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                bool wasPressed = shape.getGlobalBounds().contains(dot.x, dot.y);
                if (wasPressed) { pressedIndex = index; anyButtonPressed = true; }
            }
            else if (gs::buttonHovering && event.type == sf::Event::MouseMoved && !anyButtonPressed && !noColorChanging)
            {
                if (!wasHovered && shape.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) { pressedIndex = index; wasHovered = true; }
                else if (wasHovered && pressedIndex == index && !shape.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) wasHovered = false;
            }
            return false;
        }
        void RoundedRectangleButton::eventPolled(sf::Event& event) { if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded) { anyButtonPressed = wasHovered = false; } }
        void RoundedRectangleButton::setAlpha(const sf::Uint8& alpha)
        {
            if (loaded)
            {
                unsigned char realAlpha = sf::Uint8((unsigned char)alpha * ((float)maxAlpha/255));
                unsigned char realRAlpha = sf::Uint8((unsigned char)alpha * ((float)ralpha/255));
                text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, realAlpha));
                text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, realAlpha));
                shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, realRAlpha));
                shape.setOutlineColor(sf::Color(shape.getOutlineColor().r, shape.getOutlineColor().g, shape.getOutlineColor().b, realRAlpha));
            }
        }
        void RoundedRectangleButton::setPosition(float x, float y)
        {
            shape.setPosition(x, y);
            switch (halign)
            {
                case Halign::Left: text.setOrigin(0, text.getOrigin().y); text.setPosition(x, text.getPosition().y); break;
                case Halign::Center:
                    text.setOrigin(text.getLocalBounds().width/2, text.getOrigin().y);
                    text.setPosition(x + shape.getSize().x/2, text.getPosition().y); break;
                case Halign::Right:
                    text.setOrigin(text.getLocalBounds().width, text.getOrigin().y);
                    text.setPosition(x + shape.getSize().x, text.getPosition().y); break;
            }
            switch (valign)
            {
                case Valign::Top: text.setOrigin(text.getOrigin().x, 0); text.setPosition(text.getPosition().x, y); break;
                case Valign::Center:
                    text.setPosition(text.getPosition().x, y + shape.getSize().y/2);
                    text.setOrigin(text.getOrigin().x, 1.35*text.getLocalBounds().height/2); break;
                case Valign::Bottom:
                    text.setPosition(text.getPosition().x, y + shape.getSize().y);
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height*1.25); break;
            }
        }
        void RoundedRectangleButton::setSize(const sf::Vector2f& vec) { shape.setSize(vec); }
        void RoundedRectangleButton::setFont(const std::wstring& fontname)
        {
            if ((loaded = fc::GetFont(fontname)))
                text.setFont(*fc::GetFont(fontname));
        }
        void RoundedRectangleButton::setString(const std::wstring& string) { text.setString(string); }
        void RoundedRectangleButton::setCharacterSize(const unsigned int size)
        {
            characterSize = size;
            text.setCharacterSize(size * gs::scale);
        }
        void RoundedRectangleButton::updateColor()
        {
            if (drawOutlineShape) shape.setOutlineColor({sonormalColor.r, sonormalColor.g, sonormalColor.b, ralpha});
            if (drawOutline) text.setOutlineColor({onormalColor.r, onormalColor.g, onormalColor.b, alpha});
            shape.setFillColor({snormalColor.r, snormalColor.g, snormalColor.b, ralpha});
            text.setFillColor({normalColor.r, normalColor.g, normalColor.b, alpha});
        }
    }
}
