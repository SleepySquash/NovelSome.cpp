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
        class TextButton
        {
            std::wstring textString{ L"" };
            
            bool hovered{ false };
            bool pressed{ false };
            bool released{ false };
            bool fontLoaded{ false };
            
        public:
            sf::Text text;
            
            int maxAlpha = 255;
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 30 };
            
            enum class halignEnum { left, center, right };
            halignEnum halign = halignEnum::center;
            enum class valignEnum { top, middle, bottom };
            valignEnum valign = valignEnum::middle;
            
            int x{ 0 }, y{ 0 };
            
            void SetPosition(const int& X, const int& Y);
            void SetString(const std::wstring& line);
            void SetFont(const std::wstring& font);
            void Draw(sf::RenderWindow* window);
            void SetAlpha(const sf::Int8& alpha);
            void Resize(unsigned int width, unsigned int height);
            void PollEvent(sf::Event& event);
            bool Released();
        };
    }
}

#endif /* GUIInterface_hpp */
