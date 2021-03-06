//
//  Popup.hpp
//  NekoPlace
//
//  Created by Никита Исаенко on 03/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Popup_hpp
#define Popup_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors/Font.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

using std::cin;
using std::cout;
using std::endl;

using namespace ns;

namespace NekoNinja
{
    struct PopupInfo
    {
        std::wstring desc, imagePath;
        PopupInfo(const std::wstring& desc = L"", const std::wstring& imagePath = L"");
    };
    struct Popup : Component
    {
        sf::RectangleShape shape, backfade, line;
        std::wstring title, desc;
        GUI::TextButton button;
        sf::Text caption, info;
        
        Popup(const std::wstring& title = L"Caption", const std::wstring& desc = L"");
        void Init() override;
        void Destroy() override;
        void PollEvent(sf::Event&) override;
        void Resize(const unsigned int& width, const unsigned int& height) override;
        void Draw(sf::RenderWindow* window) override;
    };
}

#endif /* Popup_hpp */
