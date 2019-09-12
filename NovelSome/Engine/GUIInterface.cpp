//
//  GUIInterface.cpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 22/11/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "GUIInterface.hpp"

namespace ns
{
    namespace GUI
    {
        void Button::Draw(sf::RenderTarget* window) { }
        void Button::Resize(unsigned int width, unsigned int height) { }
        bool Button::PollEvent(sf::Event& event) { return false; }
        void Button::ReceiveMessage(MessageHolder& message) { }
        void Button::resetScale() { }
        void Button::setAlpha(const sf::Uint8& alpha) { }
        sf::Uint8 Button::getAlpha() { return alpha; }
        void Button::setPosition(float x, float y) { }
        void Button::setVisible(bool vis) { visible = vis; active = vis; }
        void Button::setColor(const sf::Color &fillColour) { }
        
        
        
        TextButton::TextButton()
        {
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
        }
        void TextButton::Draw(sf::RenderTarget* window) { if (fontLoaded && visible) window->draw(text); }
        void TextButton::Resize(unsigned int width, unsigned int height)
        {
            if (!regulateBounds) { bounds.width = width; bounds.height = height; }
            if (characterScale) {
                text.setCharacterSize(characterSize * gs::scScale);
                text.setOutlineThickness(thickness * gs::scScale);
            } else {
                text.setCharacterSize(characterSize * gs::scale);
                text.setOutlineThickness(thickness * gs::scale); }
            //CorrectBoundaries();
        }
        void TextButton::CorrectBoundaries()
        {
            text.setScale(1, 1);
            if (text.getGlobalBounds().width > bounds.width && text.getGlobalBounds().left < bounds.left)
                text.setScale((float)bounds.width/text.getGlobalBounds().width, 1);
            else if (text.getGlobalBounds().width - text.getOrigin().x > bounds.width - leftBound)
                text.setScale((float)(bounds.width - leftBound)/(text.getGlobalBounds().width - text.getOrigin().x), 1);
            else if (text.getGlobalBounds().left < bounds.left && text.getOrigin().x > 0)
                text.setScale((float)((text.getOrigin().x + text.getGlobalBounds().left - bounds.left))/(text.getGlobalBounds().width), 1);
            if (text.getGlobalBounds().left < bounds.left) text.setPosition(bounds.left + text.getOrigin().x*text.getScale().x, text.getPosition().y);
            /*if (text.getGlobalBounds().width > screen.x - text.getGlobalBounds().left && text.getGlobalBounds().left < 0)
                text.setScale((float)screen.x/(text.getGlobalBounds().width), 1);
            else if (text.getGlobalBounds().width > screen.x - text.getGlobalBounds().left)
                text.setScale((float)(screen.x - text.getGlobalBounds().left)/(text.getGlobalBounds().width), 1);
            else if (text.getGlobalBounds().left < 0 && text.getOrigin().x > 0)
                text.setScale((float)(text.getOrigin().x + text.getGlobalBounds().left)/(text.getGlobalBounds().width), 1);
            if (text.getGlobalBounds().left < 0) text.setPosition(text.getPosition().x - text.getGlobalBounds().left, text.getPosition().y);*/
        }
        bool TextButton::PollEvent(sf::Event& event)
        {
            if (!active) return false;
            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchEnded)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                if (wasPressed || ignoreWasPressed)
                {
                    if (!noColorChanging) {
                        text.setFillColor(sf::Color(normalColor.r, normalColor.g, normalColor.b, alpha));
                        text.setOutlineColor(sf::Color(onormalColor.r, onormalColor.g, onormalColor.b, alpha)); }
                    bool constains = text.getGlobalBounds().contains(dot.x, dot.y);
                    if (constains) event = sf::Event(); // So that no button will be clicked being the underlaying.
                    
                    wasPressed = false;
                    return constains;
                }
            }
            else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchBegan)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                wasPressed = text.getGlobalBounds().contains(dot.x, dot.y);
                if (onPress) return wasPressed;
                if (wasPressed && !noColorChanging) {
                    text.setFillColor(sf::Color(pressColor.r, pressColor.g, pressColor.b, alpha));
                    text.setOutlineColor(sf::Color(opressColor.r, opressColor.g, opressColor.b, alpha)); }
            }
            else if (gs::buttonHovering && event.type == sf::Event::MouseMoved && !wasPressed && !noColorChanging)
            {
                if (text.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                    if (!wasHovered) { text.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, alpha));
                        text.setOutlineColor(sf::Color(ohoverColor.r, ohoverColor.g, ohoverColor.b, alpha)); wasHovered = true; } }
                else if (wasHovered) { text.setFillColor(sf::Color(normalColor.r, normalColor.g, normalColor.b, alpha));
                    text.setOutlineColor(sf::Color(onormalColor.r, onormalColor.g, onormalColor.b, alpha)); wasHovered = false; }
            }
            return false;
        }
        void TextButton::resetScale() { text.setScale(1, 1); }
        void TextButton::setAlpha(const sf::Uint8& alpha)
        {
            this->alpha = sf::Uint8(alpha * ((float)maxAlpha/255));
            text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, this->alpha));
            text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, this->alpha));
        }
        sf::Uint8 TextButton::getAlpha() { return text.getOutlineColor().a; }
        void TextButton::setPosition(float x, float y)
        {
            switch (halign)
            {
                case Halign::Left: text.setOrigin(0, text.getOrigin().y); break;
                case Halign::Center: text.setOrigin(text.getLocalBounds().width/2, text.getOrigin().y); break;
                case Halign::Right: text.setOrigin(text.getLocalBounds().width, text.getOrigin().y); break;
            }
            switch (valign)
            {
                case Valign::Top: text.setOrigin(text.getOrigin().x, 0); break;
                case Valign::Center: text.setOrigin(text.getOrigin().x, text.getLocalBounds().height/2); break;
                case Valign::Bottom: text.setOrigin(text.getOrigin().x, text.getLocalBounds().height); break;
            }
            text.setPosition(x, y); CorrectBoundaries();
        }
        void TextButton::setFont(const std::wstring& fontname)
        {
            if ((fontLoaded = (fc::GetFont(fontname))))
                text.setFont(*fc::GetFont(fontname));
        }
        void TextButton::setString(const std::wstring& string) { text.setString(string); }
        void TextButton::setCharacterSize(const unsigned int size)
        {
            characterSize = size;
            text.setCharacterSize(size * gs::scale);
        }
        void TextButton::setColor(const sf::Color& fillColour) { normalColor = fillColour; text.setFillColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, text.getFillColor().a)); }
        
        
        
        SpriteButton::~SpriteButton() { if (textureName != L"") ic::DeleteImage(textureName); }
        void SpriteButton::Draw(sf::RenderTarget* window) { if (spriteLoaded && visible) window->draw(sprite); }
        void SpriteButton::Resize(unsigned int width, unsigned int height)
        {
            if (characterScale) sprite.setScale(scale * gs::scScale, scale * gs::scScale);
            else sprite.setScale(scale * gs::scale, scale * gs::scale);
        }
        bool SpriteButton::PollEvent(sf::Event& event)
        {
            if (!active) return false;
            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchEnded)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                if (wasPressed || ignoreWasPressed)
                {
                    if (!noColorChanging) sprite.setColor(sf::Color(normalColor.r, normalColor.g, normalColor.b, alpha));
                    bool constains = sprite.getGlobalBounds().contains(dot.x, dot.y);
                    if (constains) event = sf::Event(); // So that no button will be clicked being the underlaying.
                    
                    wasPressed = false;
                    return constains;
                }
            }
            else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchBegan)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                wasPressed = sprite.getGlobalBounds().contains(dot.x, dot.y);
                if (onPress) return wasPressed;
                if (wasPressed && !noColorChanging) sprite.setColor(sf::Color(pressColor.r, pressColor.g, pressColor.b, alpha));
            }
            else if (gs::buttonHovering && event.type == sf::Event::MouseMoved && !wasPressed && !noColorChanging)
            {
                if (sprite.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                    if (!wasHovered) { sprite.setColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, alpha)); wasHovered = true; } }
                else if (wasHovered) { sprite.setColor(sf::Color(normalColor.r, normalColor.g, normalColor.b, alpha)); wasHovered = false; }
            }
            return false;
        }
        void SpriteButton::ReceiveMessage(MessageHolder &message)
        {
            if (nss::Command(message.info, "Request") && message.additional == textureName)
            {
                sf::Texture* texture = ic::LoadTexture(textureName);
                if ((spriteLoaded = texture)) sprite.setTexture(*texture, true);
            }
        }
        void SpriteButton::resetScale() { sprite.setScale(1, 1); }
        void SpriteButton::setAlpha(const sf::Uint8& alpha)
        {
            this->alpha = sf::Uint8(alpha * ((float)maxAlpha/255));
            sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, this->alpha));
        }
        sf::Uint8 SpriteButton::getAlpha() { return sprite.getColor().a; }
        void SpriteButton::setPosition(float x, float y)
        {
            switch (halign)
            {
                case Halign::Left: sprite.setOrigin(0, sprite.getOrigin().y); break;
                case Halign::Center: sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getOrigin().y); break;
                case Halign::Right: sprite.setOrigin(sprite.getLocalBounds().width, sprite.getOrigin().y); break;
            }
            switch (valign)
            {
                case Valign::Top: sprite.setOrigin(sprite.getOrigin().x, 0); break;
                case Valign::Center: sprite.setOrigin(sprite.getOrigin().x, sprite.getLocalBounds().height/2); break;
                case Valign::Bottom: sprite.setOrigin(sprite.getOrigin().x, sprite.getLocalBounds().height); break;
            }
            sprite.setPosition(x, y);
        }
        void SpriteButton::setTexture(const std::wstring& imagePath, MessageSender* sender)
        {
            if (textureName.length() != 0 && textureName != L"") ic::DeleteImage(textureName);
            textureName = imagePath; spriteLoaded = false; sf::Texture* texture;
            if (sender) texture = ic::RequestHigherTexture(imagePath, sender); else texture = ic::LoadTexture(imagePath);
            if ((spriteLoaded = texture)) sprite.setTexture(*texture, true);
        }
        void SpriteButton::setTexture(sf::Texture* texture) { if ((spriteLoaded = texture)) sprite.setTexture(*texture, true); }
        void SpriteButton::setScale(const float& scl) { scale = scl; Resize(gs::width, gs::height); }
        void SpriteButton::setColor(const sf::Color& fillColour) { normalColor = fillColour; sprite.setColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, sprite.getColor().a)); }
        
        
        
        
        
        
        SpriteButtons::~SpriteButtons() { if (textureName != L"") ic::DeleteImage(textureName); }
        void SpriteButtons::Draw(sf::RenderTarget* window)
        {
            if (spriteLoaded && visible)
            {
                if (anyButtonPressed && index == pressedIndex) sprite.setColor(sf::Color::Yellow);
                window->draw(sprite);
                if (anyButtonPressed && index == pressedIndex) sprite.setColor(sf::Color::White);
            }
        }
        void SpriteButtons::Resize(unsigned int width, unsigned int height)
        {
            if (characterScale) sprite.setScale(scale * gs::scScale, scale * gs::scScale);
            else sprite.setScale(scale * gs::scale, scale * gs::scale);
        }
        bool SpriteButtons::PollEvent(sf::Event& event)
        {
            if (!active) return false;
            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchEnded)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                if (pressedIndex == index && (anyButtonPressed || ignoreWasPressed))
                {
                    sprite.setColor(sf::Color::White);
                    bool constains = sprite.getGlobalBounds().contains(dot.x, dot.y);
                    if (constains) { pressedIndex = -1; anyButtonPressed = false; event = sf::Event(); }
                    
                    return constains;
                }
            }
            else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchBegan)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                bool wasPressed = sprite.getGlobalBounds().contains(dot.x, dot.y);
                if (wasPressed) { pressedIndex = index; anyButtonPressed = true; }
            }
            return false;
        }
        void SpriteButtons::ReceiveMessage(MessageHolder &message)
        {
            if (nss::Command(message.info, "Request") && message.additional == textureName)
            {
                sf::Texture* texture = ic::LoadTexture(textureName);
                if ((spriteLoaded = texture)) sprite.setTexture(*texture, true);
            }
        }
        void SpriteButtons::eventPolled(sf::Event& event) { if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded) anyButtonPressed = false; }
        void SpriteButtons::resetScale() { sprite.setScale(1, 1); }
        void SpriteButtons::setAlpha(const sf::Uint8& alpha)
        {
            this->alpha = sf::Uint8(alpha * ((float)maxAlpha/255));
            sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, this->alpha));
        }
        sf::Uint8 SpriteButtons::getAlpha() { return sprite.getColor().a; }
        void SpriteButtons::setPosition(float x, float y)
        {
            switch (halign)
            {
                case Halign::Left: sprite.setOrigin(0, sprite.getOrigin().y); break;
                case Halign::Center: sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getOrigin().y); break;
                case Halign::Right: sprite.setOrigin(sprite.getLocalBounds().width, sprite.getOrigin().y); break;
            }
            switch (valign)
            {
                case Valign::Top: sprite.setOrigin(sprite.getOrigin().x, 0); break;
                case Valign::Center: sprite.setOrigin(sprite.getOrigin().x, sprite.getLocalBounds().height/2); break;
                case Valign::Bottom: sprite.setOrigin(sprite.getOrigin().x, sprite.getLocalBounds().height); break;
            }
            sprite.setPosition(x, y);
        }
        void SpriteButtons::setTexture(const std::wstring& imagePath, MessageSender* sender)
        {
            if (textureName.length() != 0 && textureName != L"") ic::DeleteImage(textureName);
            textureName = imagePath; spriteLoaded = false; sf::Texture* texture;
            if (sender) texture = ic::RequestHigherTexture(imagePath, sender); else texture = ic::LoadTexture(imagePath);
            if ((spriteLoaded = texture)) sprite.setTexture(*texture, true);
        }
        void SpriteButtons::setTexture(sf::Texture* texture) { if ((spriteLoaded = (texture != nullptr))) sprite.setTexture(*texture, true); }
        void SpriteButtons::setScale(const float& scl) { scale = scl; Resize(gs::width, gs::height); }
        void SpriteButtons::setColor(const sf::Color& fillColour) { sprite.setColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, sprite.getColor().a)); }
        
        
        
        
        
        
        RectangleButton::RectangleButton()
        {
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
            shape.setFillColor(sf::Color::White);
            shape.setOutlineColor(sf::Color::Black);
        }
        void RectangleButton::Draw(sf::RenderTarget* window) { if (fontLoaded && visible) { window->draw(shape); window->draw(text); } }
        void RectangleButton::Resize(unsigned int width, unsigned int height)
        {
            if (characterScale)
            {
                text.setCharacterSize(characterSize * gs::scScale);
                text.setOutlineThickness(thickness * gs::scScale);
            } else {
                text.setCharacterSize(characterSize * gs::scale);
                text.setOutlineThickness(thickness * gs::scale);
            }
            text.setScale(1, 1);
            if (shape.getSize().x < text.getGlobalBounds().width)
                text.setScale(shape.getSize().x/(text.getGlobalBounds().width), 1);
        }
        bool RectangleButton::PollEvent(sf::Event& event)
        {
            if (!active) return false;
            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchEnded)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                if (wasPressed || ignoreWasPressed)
                {
                    text.setFillColor(sf::Color::White); shape.setFillColor(sf::Color::White);
                    bool constains = shape.getGlobalBounds().contains(dot.x, dot.y);
                    if (constains) event = sf::Event(); // So that no button will be clicked being the underlaying.
                    
                    wasPressed = false;
                    return constains;
                }
            }
            else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchBegan)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                wasPressed = shape.getGlobalBounds().contains(dot.x, dot.y);
                if (onPress) return wasPressed;
                if (wasPressed) { shape.setFillColor(sf::Color::Yellow); text.setFillColor(sf::Color::Yellow); }
            }
            return false;
        }
        void RectangleButton::resetScale() { text.setScale(1, 1); }
        void RectangleButton::setAlpha(const sf::Uint8& alpha)
        {
            this->alpha = sf::Uint8(alpha * ((float)maxAlpha/255));
            text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, this->alpha));
            text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, this->alpha));
            shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, this->alpha));
            shape.setOutlineColor(sf::Color(shape.getOutlineColor().r, shape.getOutlineColor().g, shape.getOutlineColor().b, this->alpha));
        }
        sf::Uint8 RectangleButton::getAlpha() { return text.getOutlineColor().a; }
        void RectangleButton::setPosition(float x, float y)
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
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height/2); break;
                case Valign::Bottom:
                    text.setPosition(text.getPosition().x, y + shape.getSize().y);
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height); break;
            }
        }
        void RectangleButton::setSize(const sf::Vector2f& vec) { shape.setSize(vec); }
        void RectangleButton::setFont(const std::wstring& fontname)
        {
            if ((fontLoaded = (fc::GetFont(fontname) != nullptr)))
                text.setFont(*fc::GetFont(fontname));
        }
        void RectangleButton::setString(const std::wstring& string) { text.setString(string); }
        void RectangleButton::setCharacterSize(const unsigned int size)
        {
            characterSize = size;
            text.setCharacterSize(size * gs::scale);
        }
        void RectangleButton::setColor(const sf::Color& fillColour) { text.setFillColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, text.getFillColor().a)); }
        
        
        
        
        
        
        RectangleButtons::RectangleButtons()
        {
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
            if (drawShape) {
                shape.setFillColor(shapeFillColor);
                shape.setOutlineColor(sf::Color::White); }
        }
        void RectangleButtons::Draw(sf::RenderTarget* window)
        {
            if (fontLoaded && visible)
            {
                if (shape.getSize().x < text.getGlobalBounds().width) { text.setScale(shape.getSize().x/(text.getGlobalBounds().width), 1); }
                if (anyButtonPressed && index == pressedIndex) {
                    shape.setFillColor(sf::Color(normalColor.r, normalColor.g, normalColor.b, alpha));
                    text.setFillColor(sf::Color(pressColor.r, pressColor.g, pressColor.b, alpha)); }
                if (drawShape) window->draw(shape); window->draw(text);
                if (anyButtonPressed && index == pressedIndex) {
                    shape.setFillColor(sf::Color(shapeFillColor.r, shapeFillColor.g, shapeFillColor.b, 170 * ((float)alpha/maxAlpha) ));
                    text.setFillColor(sf::Color(normalColor.r, normalColor.g, normalColor.b, alpha)); }
                text.setScale(1, 1);
            }
        }
        void RectangleButtons::Resize(unsigned int width, unsigned int height)
        {
            if (characterScale)
            {
                text.setCharacterSize(characterSize * gs::scScale);
                text.setOutlineThickness(thickness * gs::scScale);
                if (drawShape) shape.setOutlineThickness(thickness * gs::scScale);
            } else {
                text.setCharacterSize(characterSize * gs::scale);
                text.setOutlineThickness(thickness * gs::scale);
                if (drawShape) shape.setOutlineThickness(thickness * gs::scale);
            }
            text.setScale(1, 1);
        }
        bool RectangleButtons::PollEvent(sf::Event& event)
        {
            if (!active) return false;
            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchEnded)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                if (pressedIndex == index && (anyButtonPressed || ignoreWasPressed))
                {
                    // text.setFillColor(sf::Color::White); shape.setFillColor(shapeFillColor);
                    bool constains = (drawShape ? shape.getGlobalBounds().contains(dot.x, dot.y) : text.getGlobalBounds().contains(dot.x, dot.y));
                    if (constains) { pressedIndex = -1; anyButtonPressed = false; event = sf::Event(); }

                    return constains;
                }
            }
            else if ((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) || event.type == sf::Event::TouchBegan)
            {
                if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y };
                else dot = { event.touch.x, event.touch.y };
                
                bool wasPressed = (drawShape ? shape.getGlobalBounds().contains(dot.x, dot.y) : text.getGlobalBounds().contains(dot.x, dot.y));
                if (wasPressed) { pressedIndex = index; anyButtonPressed = true; }
            }
            return false;
        }
        void RectangleButtons::eventPolled(sf::Event& event) { if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::TouchEnded) anyButtonPressed = false; }
        void RectangleButtons::resetScale() { text.setScale(1, 1); }
        void RectangleButtons::setAlpha(const sf::Uint8& alpha)
        {
            this->alpha = sf::Uint8(alpha * ((float)maxAlpha/255));
            text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, this->alpha));
            text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, this->alpha));
            if (drawShape)
            {
                shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, this->alpha));
                shape.setOutlineColor(sf::Color(shape.getOutlineColor().r, shape.getOutlineColor().g, shape.getOutlineColor().b, this->alpha));
            }
        }
        sf::Uint8 RectangleButtons::getAlpha() { return text.getOutlineColor().a; }
        void RectangleButtons::setPosition(float x, float y)
        {
            if (drawShape) shape.setPosition(x, y);
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
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height/2); break;
                case Valign::Bottom:
                    text.setPosition(text.getPosition().x, y + shape.getSize().y);
                    text.setOrigin(text.getOrigin().x, text.getLocalBounds().height); break;
            }
        }
        void RectangleButtons::setSize(const sf::Vector2f& vec) { shape.setSize(vec); }
        void RectangleButtons::setFont(const std::wstring& fontname)
        {
            sf::Font* font = fc::GetFont(fontname);
            if ((fontLoaded = font)) text.setFont(*font);
        }
        void RectangleButtons::setString(const std::wstring& string) { text.setString(string); }
        void RectangleButtons::setCharacterSize(const unsigned int size)
        {
            characterSize = size;
            text.setCharacterSize(size * gs::scale);
        }
        void RectangleButtons::setColor(const sf::Color& fillColour) { text.setFillColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, text.getFillColor().a)); }
        
        
        
        
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
