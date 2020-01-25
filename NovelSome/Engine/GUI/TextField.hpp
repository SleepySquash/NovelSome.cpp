//
//  TextField.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 24.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef TextField_hpp
#define TextField_hpp

#include <SFML/Graphics.hpp>
#include "../Settings.hpp"
#include "../Collectors/Font.hpp"
#include "enums.hpp"

namespace ns
{
    namespace GUI
    {
        struct TextField
        {
            sf::Text text;
            bool active{ false }, wasActive{ false }, fontLoaded{ false }, drawWhenEmpty{ false }, enteredSomething{ false };
            sf::RectangleShape shape; sf::String string;
            std::wstring textWhenEmpty;
            unsigned int characterSize{ 20 };
            bool crypto{ false };
            
            bool drawBlink{ true };
            float elapsedBlink{ 0.f };
            
            void Init();
            bool PollEvent(sf::Event& event);
            void Resize(const unsigned int& width, const unsigned int& height);
            void Draw(sf::RenderWindow* window);
            void setFont(const std::wstring& fontname);
            void setPosition(float x, float y);
            bool setActive(const bool& act);
        };
        
        struct Tick
        {
            
        };
    }
}

#endif /* TextField_hpp */
