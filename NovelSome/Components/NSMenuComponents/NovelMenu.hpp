//
//  NovelMenu.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 04/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#ifndef NovelMenu_hpp
#define NovelMenu_hpp

#include <iostream>

#include <locale>
#include <fstream>
#include <codecvt>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "NovelsLibrary.hpp"
#include "../NovelComponents/GUISystem.hpp"

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace NSMenuComponents
    {
        struct NovelMenu : public Component
        {
            NovelComponents::GUISystem guiSystem;
            GUI::TextButton backToMenu;
            
            bool active{ true };
            NovelInfo* nvl; std::wstring menuPath, scenario;
            
            NovelMenu(const std::wstring& menuPath, const std::wstring& scenario, NovelInfo* nvl);
            void Init() override;
            void PollEvent(sf::Event& event) override;
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Draw(sf::RenderWindow* window) override;
            void ReceiveMessage(MessageHolder& message) override;
        };
    }
}

#endif /* NovelMenu_hpp */
