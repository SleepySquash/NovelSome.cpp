//
//  GamePause.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 10/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#ifndef GamePause_hpp
#define GamePause_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/List.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/NovelSystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

#include "../GUISystem.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct GamePause : Component
        {
            sf::RectangleShape shape;
            GUI::TextButton menuBackButton;
            
            sf::Int8 alpha{ 0 }; int maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.15f }, disappearTime{ 0.15f };
            
            GUISystem* guiSystem{ nullptr };
            enum modeEnum{ appearing, waiting, appeared, disappearing };
            modeEnum mode{ waiting };
            
            bool countdownLastTouchedMoment{ false };
            float lastTouchedMoment{ 0.f };
            
            GamePause(GUISystem* guiSystem = nullptr);
            ~GamePause();
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow *window) override;
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void ReceiveMessage(MessageHolder& message) override;
        };
    }
}

#endif /* GamePause_hpp */
