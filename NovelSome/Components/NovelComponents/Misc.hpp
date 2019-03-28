//
//  Misc.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Misc_hpp
#define Misc_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "GUISystem.hpp"

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
            void Resize(unsigned int width, unsigned int height) override;
        };
        
        
        
        
        
        
        struct Waiting : NovelObject
        {
            float currentTime{ 0.f }, waitingTime{ 1.f };
            void Update(const sf::Time& elapsedTime) override;
        };
    }
}

#endif /* Misc_h */
