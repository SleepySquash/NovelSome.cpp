//
//  Button.cpp
//  NekoUI
//
//  Created by Никита Исаенко on 18.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#include "Button.hpp"

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
        void Button::updateColor() { }
    }
}
