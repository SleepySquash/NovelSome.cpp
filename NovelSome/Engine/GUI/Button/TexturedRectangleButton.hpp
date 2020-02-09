//
//  TexturedRectangleButton.hpp
//  NekoUI
//
//  Created by Никита Исаенко on 24.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef TexturedRectangleButton_hpp
#define TexturedRectangleButton_hpp

#include "Button.hpp"
#include "../RoundedRectangleShape.hpp"
#include "../../Collectors/Image.hpp"

namespace ns
{
    namespace GUI
    {
        struct TexturedRectangleButtons : Button
        {
            bool anyButtonPressed{ false }, drawShape{ true };
            unsigned long index{ 0 }, pressedIndex{ 0 };
            
            sf::RoundedRectangleShape shape;
            bool drawOutline{ true };
            sf::Color onormalColor{ sf::Color::Black },
                      ohoverColor{ 140, 140, 140 },
                      opressColor{ sf::Color::Black };
            sf::Color snormalColor{ sf::Color::White },
                      shoverColor{ 225, 225, 225 },
                      spressColor{ 200, 200, 200 };
            bool drawOutlineShape{ true };
            sf::Color sonormalColor{ sf::Color::White },
                      sohoverColor{ 40, 40, 40 },
                      sopressColor{ sf::Color::White };
            sf::Uint8 ralpha{ 255 };
            
            bool spriteLoaded{ false };
            std::wstring textureName{ L"" };
            
            sf::Text text;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f }, radius{ 20 };
            
            TexturedRectangleButtons();
            ~TexturedRectangleButtons();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void eventPolled(sf::Event& event);
            void ReceiveMessage(MessageHolder& message) override;
            void setAlpha(const sf::Uint8& alpha) override;
            void setPosition(float x, float y) override;
            void setSize(const sf::Vector2f& vec);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void setTexture(const std::wstring& imagePath, MessageSender* sender = nullptr);
            void updateColor() override;
        };
    }
}

#endif /* TexturedRectangleButton_hpp */
