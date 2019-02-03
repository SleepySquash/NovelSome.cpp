//
//  GUIInterface.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/10/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef GUIInterface_hpp
#define GUIInterface_hpp

#include <iostream>

#include <SFML/Graphics.hpp>

#include "../Essentials/Base.hpp"
#include "StaticMethods.hpp"

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace GUI
    {
        struct TextButton
        {
        public:
            bool fontLoaded{ false };
            
            bool onPress{ false };
            bool wasPressed{ false };
            bool ignoreWasPressed{ false };
            sf::Vector2i dot;
            
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 30 };
            float thickness{ 1.f };
            int maxAlpha = 255;
            
            enum class halignEnum { left, center, right };
            enum class valignEnum { top, center, bottom };
            halignEnum halign{ halignEnum::center };
            valignEnum valign{ valignEnum::top };
            
            TextButton();
            void Update(const sf::Time& elapsedTime);
            void Draw(sf::RenderTarget* window);
            void Resize(unsigned int width, unsigned int height);
            bool PollEvent(sf::Event& event);
            void setAlpha(const sf::Int8& alpha);
            void setPosition(float x, float y);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
        };
    }
}

#endif /* GUIInterface_hpp */
