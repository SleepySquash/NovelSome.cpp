//
//  RoundedRectangleButton.hpp
//  NekoUI
//
//  Created by Никита Исаенко on 29.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef RoundedRectangleButton_hpp
#define RoundedRectangleButton_hpp

#include "Button.hpp"
#include "../RoundedRectangleShape.hpp"
#include "../../Collectors/Image.hpp"

namespace ns
{
    namespace GUI
    {
        struct RoundedRectangleButton : Button
        {
            bool anyButtonPressed{ false }, drawShape{ true };
            unsigned long index{ 0 }, pressedIndex{ 0 };
            
            sf::RoundedRectangleShape shape;
            bool drawOutline{ true };
            sf::Color onormalColor{ sf::Color::Black },
                      ohoverColor{ 140, 140, 140 },
                      opressColor{ sf::Color::Black };
            sf::Color snormalColor{ sf::Color::Black },
                      shoverColor{ 225, 225, 225 },
                      spressColor{ 200, 200, 200 };
            bool drawOutlineShape{ true };
            sf::Color sonormalColor{ sf::Color::White },
                      sohoverColor{ 40, 40, 40 },
                      sopressColor{ sf::Color::White };
            sf::Uint8 ralpha{ 255 };
            
            sf::Text text;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f }, radius{ 20 };
            
            RoundedRectangleButton();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void eventPolled(sf::Event& event);
            void setAlpha(const sf::Uint8& alpha) override;
            void setPosition(float x, float y) override;
            void setSize(const sf::Vector2f& vec);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void updateColor() override;
        };
    }
}

#endif /* RoundedRectangleButton_hpp */
